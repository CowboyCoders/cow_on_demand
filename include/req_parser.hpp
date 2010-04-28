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

#ifndef ___REQ_PARSER___
#define ___REQ_PARSER___

#include <string>

#include <boost/regex.hpp>
#include <boost/tokenizer.hpp>

/**
 * Request parser responsible for handling the HTTP side of things.
 */
class req_parser {
public:
    req_parser()
        : size_(0), 
          indices_(),
          consumer_state(line),
          parser_state(http_get),
          // regular expressions
          http_get_expr("GET /(\\S+) HTTP/1.(?:0|1)"),
          size_header_expr("[Ss]ize: ?([0-9]+)"),
          indices_header_expr("[Ii]ndices: (([0-9]+[ ]{0,1})+)"),
          empty_line_expr("^\\s*$")
    {
    }

    /**
     * Consumes a single character.
     * Returns true if everything worked
     * 
     * @return true if the char could be consumed, otherwise false
     */
    bool consume(char c);

    /**
     * The parsed list of indices to use as response.
     *
     * @return The indices which should be read 
     */
    std::vector<size_t> indices() const
    {
        return indices_;
    }

    /**
     * Requested file name.
     *
     * @return The request file name
     */
    std::string file() const
    {
        return file_;
    }

    /**
     * Supplied size parameter.
     *
     * @return The size of piece
     */
    size_t size() const
    {
        return size_;
    }

private:
    bool parse(const std::string& line); 

    std::string file_;
    size_t size_;
    std::vector<size_t> indices_;

    std::string current_line;

    enum consumer_states {
        line = 1,
        first_cr,
        first_nl,
        second_cr
    };

    enum parser_states {
        http_get = 1,
        headers
    };

    consumer_states consumer_state;
    parser_states parser_state;

    boost::regex http_get_expr;
    boost::regex size_header_expr; 
    boost::regex index_header_expr;
    boost::regex indices_header_expr;
    boost::regex count_header_expr;
    boost::regex empty_line_expr;
};

#endif
