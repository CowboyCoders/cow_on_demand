/*
Copyright 2010 CowboyCoders. All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY COWBOYCODERS ``AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL COWBOYCODERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of CowboyCoders.
*/

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
