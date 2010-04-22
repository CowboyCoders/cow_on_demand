#include <iostream>
#include <req_parser.hpp>


int main() {
    
    req_parser parser;

    std::string str = "GET /shit HTTP/1.1\r\nSize: 10\r\nIndices: 2 3 4\r\n\r\n";
    std::string::iterator it;
    for(it = str.begin(); it != str.end(); ++it) {
        if(!parser.consume(*it)) {
            std::cerr << "parser test FAILED!" << std::endl;
            return 1;
        }
    }

    std::cout << "file: " << parser.file() << std::endl;
    std::cout << "size: " << parser.size() << std::endl;
    std::cout << "indices: ";
    std::vector<size_t> indices = parser.indices();
    for(std::vector<size_t>::iterator it = indices.begin();
        it != indices.end(); 
        ++it) {
        std::cout << *it << " "; 
    }
    std::cout << std::endl;

    return 0;
}
