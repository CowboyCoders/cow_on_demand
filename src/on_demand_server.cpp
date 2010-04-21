#include <iostream>

#include "server.hpp"

/**
 * On demand server main
 */
int main(int argc, char* argv[]) {
    if(argc != 4) {
        std::cerr << "Usage: on_demand_server <ip> <port> <document root>" << std::endl;
        return 1;
    }

    server s(argv[1], argv[2], argv[3], 5);

    s.run();

    return 0;
}
