#include <iostream>

#include "server.hpp"

/**
 * On demand server main
 */
int main(int argc, char* argv[]) {
    if(argc != 3) {
        std::cerr << "usage: ip port" << std::endl;
        return 1;
    }

    server s(argv[1], argv[2], 5);

    s.run();

    return 0;
}
