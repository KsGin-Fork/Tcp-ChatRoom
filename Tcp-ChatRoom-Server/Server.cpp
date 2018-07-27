//
// Created by ksgin on 18-7-27.
//

#include <iostream>
#include "Server.h"

using namespace boost;
using namespace asio;
using namespace system;
using namespace ip;
using namespace std;

Server::Server(const int &port) : port(port) {
    ep = tcp::endpoint(tcp::v4() , port);
    msg_queue = boost::lockfree::queue<std::pair<const_buffer, socket_ptr>,boost::lockfree::fixed_sized<false>>();
    user_map = unordered_map<string , ip::tcp::socket>();
}

Server::Server() {
    ep = tcp::endpoint(tcp::v4() , 5555);
    msg_queue = boost::lockfree::queue<std::pair<const_buffer, socket_ptr>,boost::lockfree::fixed_sized<false>>();
    user_map = unordered_map<string , ip::tcp::socket>();
}

void Server::Listen() {
    boost::thread(bind(&Server::write_handler , this));
    while (true) {
        socket_ptr sock(new ip::tcp::socket(service));
        ip::tcp::acceptor acc(service , ep);
        acc.accept(*sock);
        boost::thread(bind(&Server::read_handler , this , sock));
    }
}
void Server::shutdown() {

}

void Server::read_handler(socket_ptr sock) {
    auto remote_address = sock->remote_endpoint().address();
    auto remote_port = sock->remote_endpoint().port();
    write(*sock, buffer("Ready"), ecode);
    while (true) {
        char data[512];
        size_t len = sock->read_some(buffer(data), ecode);
        if (len > 0) {
            std::cout << remote_address << "/" << remote_port << ": " << data << std::endl;
            pair<const_buffer, socket_ptr> sm(buffer("") , sock);
            msg_queue.push(sm);
        }
    }
}

void Server::write_handler() {
    while (true) {
        if (!msg_queue.empty()){
            pair<const_buffer, socket_ptr> sm(buffer("") , nullptr);
            msg_queue.pop(sm);
            write(*sm.second , sm.first , this->ecode);
        }
    }
}





