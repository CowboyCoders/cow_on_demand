#include <iostream>
#include "req_parser.h"


int main() {
    
    ReqParser parser;

    std::string str = "GET /shit HTTP/1.1\r\nSize: 10\r\nIndex: 2\r\nCount: 4\r\n\r\n";
    std::string::iterator it;
    for(it = str.begin(); it != str.end(); ++it) {
        if(!parser.consume(*it)) {
            std::cerr << "parser test FAILED!" << std::endl;
            return 1;
        }
    }

    std::cout << "file: " << parser.file() << std::endl;
    std::cout << "size: " << parser.size() << std::endl;
    std::cout << "index: " << parser.index() << std::endl;
    std::cout << "count: " << parser.count() << std::endl;

    return 0;
}
