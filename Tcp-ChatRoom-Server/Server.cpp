//
// Created by ksgin on 18-7-27.
//

#include <iostream>
#include "Server.h"

using namespace boost;
using namespace asio;
using namespace system;
using namespace std;

Server::Server(const int &port) {
    this->port = port;
    ep = ip::tcp::endpoint(ip::tcp::v4(), this->port);
    msg_queue = new lockfree::queue<pair<const_buffer, socket_ptr> *>(256);
    user_map = new unordered_map<string, ip::tcp::socket>();
}

Server::Server() {
    ep = ip::tcp::endpoint(ip::tcp::v4(), 5555);
    msg_queue = new lockfree::queue<pair<const_buffer, socket_ptr> *>(256);
    user_map = new unordered_map<string, ip::tcp::socket>();
}

void Server::Listen() {
    boost::thread write_thread(bind(&Server::write_handler, this));
    std::cout << "开始监听 3002 端口..." << std::endl;
    while (true) {
        socket_ptr sock(new ip::tcp::socket(service));
        ip::tcp::acceptor acc(service, ep);
        acc.accept(*sock);
        boost::thread read_thread(bind(&Server::read_handler, this, sock));
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
    auto remote_address = sock->remote_endpoint().address();
    auto remote_port = sock->remote_endpoint().port();
    std::cout << remote_address << "/" << remote_port << "已连接"<< std::endl;
    write(*sock, buffer("Ready"), ecode);
    while (true) {
        char data[512];
        size_t len = sock->read_some(buffer(data), ecode);
        if (len > 0) {
            std::cout << remote_address << "/" << remote_port << ": " << data << std::endl;
            auto sm = new pair<const_buffer, socket_ptr>(buffer(data), sock);
            msg_queue->push(sm);
        }
    }
}

void Server::write_handler() {
    while (true) {
        if (!msg_queue->empty()) {
            auto sm = new pair<const_buffer, socket_ptr>(buffer(""), nullptr);
            msg_queue->pop(sm);
            write(*sm->second, sm->first, this->ecode);
            delete sm;
            sm = nullptr;
        }
    }
}





