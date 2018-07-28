//
// Created by ksgin on 18-7-27.
//

#ifndef TCP_CHATROOM_SERVER_SERVER_H
#define TCP_CHATROOM_SERVER_SERVER_H

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/lockfree/queue.hpp>
#include <unordered_map>

typedef std::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr;

/**
 * Server 类
 */

class Server {
private:

    /**
     * 待发送消息队列 ，所有 socket 线程向里边写 ，发送线程读
    */
    boost::lockfree::queue<std::pair<boost::asio::const_buffer, socket_ptr> *> *msg_queue;
    /**
     * 在线用户对照表
     */
    std::unordered_map<std::string, socket_ptr> *user_map;
    /**
     * service
     */
    boost::asio::io_service service;
    /**
     * error_code
     */
    boost::system::error_code ecode;

    /**
     * port
     */
    int port;

    /**
     * endpoint
     */
    boost::asio::ip::tcp::endpoint ep;

    /**
     * 发送线程
    */
    void write_handler();

    /**
     * 接收线程
     */
    void read_handler(socket_ptr socket);

public:

    /**
     * 构造方法
     * @param port 端口号
     */
    Server(const int &port);

    /**
     * 默认构造方法
     */
    Server();

    /**
     * 监听
     */
    void Listen();

    /**
     * 关闭
     */
    void shutdown();

};

#endif //TCP_CHATROOM_SERVER_SERVER_H
