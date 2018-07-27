#include <iostream>
#include "Server.h"

int main() {
    Server *server = new Server(3002);
    server->Listen();
    return 0;
}