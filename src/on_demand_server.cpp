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

    // Make sure the document root ends with a trailing slash
    std::string root(argv[3]);
    if(root.find_last_of('/') != root.size() - 1) {
        root = root + "/";
    }

    server s(argv[1], argv[2], root, 5);

    s.run();

    return 0;
}
