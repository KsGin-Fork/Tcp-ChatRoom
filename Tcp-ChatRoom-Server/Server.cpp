//
// Created by ksgin on 18-7-27.
//

#include "Server.h"

using namespace boost;
using namespace asio;
using namespace system;
using namespace ip;
using namespace std;

Server::Server(const int &port) : port(port) {
    ep = tcp::endpoint(tcp::v4() , port);
    msg_queue = boost::lockfree::queue<std::pair<const_buffer, ip::tcp::socket>>();
    user_map = unordered_map<string , ip::tcp::socket>();
}

Server::Server() {
    ep = tcp::endpoint(tcp::v4() , 5555);
    msg_queue = boost::lockfree::queue<std::pair<boost::asio::const_buffer, boost::asio::ip::tcp::socket>>();
    user_map = unordered_map<string , ip::tcp::socket>();
}

void Server::Listen() {
    socket_ptr sock(new ip::tcp::socket(service));
    ip::tcp::acceptor acc(service , ep);
    acc.accept(*sock);
    boost::thread(bind(read_handler , sock));
}
void Server::shutdown() {

}

void Server::read_handler(socket_ptr socket) {

}

void Server::write_handler() {
    if (!msg_queue.empty()){

    }
}





