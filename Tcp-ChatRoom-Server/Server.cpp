//
// Created by ksgin on 18-7-27.
//

#include <iostream>
#include <boost/algorithm/string.hpp>
#include <string>
#include <vector>
#include "Server.h"

using namespace boost;
using namespace asio;
using namespace system;
using namespace std;

Server::Server(const int &port) {
    this->port = port;
    ep = ip::tcp::endpoint(ip::tcp::v4(), this->port);
    msg_queue = new lockfree::queue<std::pair<const_buffer, socket_ptr> *>(256);
    user_map = new unordered_map<string, socket_ptr>();
}

Server::Server() {
    ep = ip::tcp::endpoint(ip::tcp::v4(), 5555);
    msg_queue = new lockfree::queue<pair<const_buffer, socket_ptr> *>(256);
    user_map = new unordered_map<string, socket_ptr>();
}

void Server::Listen() {
    boost::thread(bind(&Server::write_handler, this));
    std::cout << "开始监听 3002 端口..." << std::endl;
    while (true) {
        socket_ptr sock(new ip::tcp::socket(service));
        ip::tcp::acceptor acc(service, ep);
        acc.accept(*sock);
        boost::thread(bind(&Server::read_handler, this, sock));
    }
}

void Server::shutdown() {
    if (this->msg_queue) {
        delete this->msg_queue;
        this->msg_queue = nullptr;
    }
    if (this->user_map) {
        delete this->user_map;
        this->user_map = nullptr;
    }
}

void Server::read_handler(socket_ptr sock) {
    bool isLoginSuccessful = false;
    auto remote_address = sock->remote_endpoint().address();
    auto remote_port = sock->remote_endpoint().port();
    msg_queue->push(new pair<const_buffer, socket_ptr>(buffer("已连接聊天室...请输入你的昵称："), sock));
    char data[512];
    string user;
    while (true) {
        memset(data , 0 , 512);
        size_t len = sock->read_some(buffer(data), ecode);
        if (len > 0) {
            std::cout << remote_address << "/" << remote_port << ": " << data << std::endl;
            if (isLoginSuccessful) {
                msg_handler(sock , string(data) , user);
            } else {
                if (user_map->find(string(data)) != user_map->end()) {
                    msg_queue->push(new pair<const_buffer, socket_ptr>(buffer("昵称已存在，请重新输入："), sock));
                } else {
                    user = to_string(data);
                    user_map->insert(make_pair(user, sock));
                    string respond = "登录成功\n 欢迎来到本聊天室,聊天室规则如下：\n\t 输入 0 获取所有用户在线状态 \n\t 输入 1 用户名 信息 给对应用户发送信息";
                    msg_queue->push(new pair<const_buffer, socket_ptr>(buffer(respond), sock));
                    isLoginSuccessful = true;
                }

            }

        }
    }
}

void Server::write_handler() {
    while (true) {
        if (!msg_queue->empty()) {
            auto sm = new pair<const_buffer, socket_ptr>(buffer(""), nullptr);
            msg_queue->pop(sm);
            sm->second.get()->send(sm->first);
            delete sm;
            sm = nullptr;
        }
    }
}

void Server::msg_handler(socket_ptr &sock, const string &msg , const std::string &user_name) {
    vector<string> strs;
    split(strs , msg , is_any_of(" ") , token_compress_on);
    stringstream respond;
    if (strs.size() == 1 && strs[0][0] - '0' == GET_ALL_USER) {
        respond << "\n当前在线用户：\n";
        for (auto &user : *user_map) {
            respond << "\t" + user.first << " " <<
                    user.second.get()->remote_endpoint().address().to_string() + ":" <<
                    user.second.get()->remote_endpoint().port() << "\n";
        }
        msg_queue->push(new pair<const_buffer, socket_ptr>(buffer(respond.str()), sock));
    } else if (strs.size() == 3 && strs[0][0] - '0' == SEND_MESSAGE) {
        if (user_map->find(strs[1]) != user_map->end()) {
            string str = user_name;
            str.append("对你说");
            str.append(strs[2]);
            msg_queue->push(new pair<const_buffer, socket_ptr>(buffer(str), (*user_map)[strs[1]]));
            msg_queue->push(new pair<const_buffer, socket_ptr>(buffer("发送成功"), sock));
        } else {
            msg_queue->push(new pair<const_buffer, socket_ptr>(buffer("用户不存在或者已下线"), sock));
        }

    } else {
        msg_queue->push(new pair<const_buffer, socket_ptr>(buffer("消息格式错误"), sock));
    }

}




