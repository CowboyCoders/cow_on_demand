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

#include <cstdlib> // for atio
#include <boost/algorithm/string.hpp> // for boost::split
#include <boost/log/trivial.hpp>
#include <sstream>
#include "req_parser.hpp"

/**
 * Parser state management.
 */
bool req_parser::consume(char c)
{
    if(consumer_state == line) {
        if(c == '\r') {
            parse(current_line);
            consumer_state = first_cr;
            return true;
        } else if(c == '\n') {
            BOOST_LOG_TRIVIAL(error) << "parser error: newline character not expected";
            return false;
        } else {
            current_line.push_back(c);
            return true;
        }
    } else if(consumer_state == first_cr) {
        if(c == '\n') {
            consumer_state = first_nl;
            return true;
        } else {
            BOOST_LOG_TRIVIAL(error) << "parser error: expected newline after carriage return";
            return false;
        }

    } else if(consumer_state == first_nl) {
        if(c == '\r') {
            consumer_state = second_cr;
        } else {
            // start of new line
            consumer_state = line;
            current_line.clear();
            current_line.push_back(c);
        }

        return true;
    } else if(consumer_state == second_cr) {
        if(c == '\n') {
            return true;
        } else {
            BOOST_LOG_TRIVIAL(error) << "parser error: expected a terminating newline";
            return false;
        }
    } else {
        BOOST_LOG_TRIVIAL(error)<< "parser error: undefined consumer state";
        return false;
    }
}

/**
 * Extracts actionable information from the request.
 */
bool req_parser::parse(const std::string& line)
{
    if(parser_state == http_get) {
        boost::smatch matches;

        if(boost::regex_match(current_line, matches, http_get_expr)) {
            if(matches.size() == 2) {
                file_ = matches[1];
                parser_state = headers;
                BOOST_LOG_TRIVIAL(debug) << "parser: header parsed sucessfully";
                return true;
            } else {
                BOOST_LOG_TRIVIAL(error) << "parser error: wrong number of matches "
                          << "for http_get_expr pattern";
                return false; 
            }
        } else if(boost::regex_match(current_line, empty_line_expr)) {
            return true;
        } else {
            BOOST_LOG_TRIVIAL(error) << "parser error: malformed http get request";
            return false;
        }
    } else if(parser_state == headers) {
        boost::smatch matches;
        
        // Size parameter
        if(boost::regex_match(current_line, matches, size_header_expr)) {
            if(matches.size() == 2) {
                std::string match = matches[1];
                size_ = atoi(match.c_str());
                BOOST_LOG_TRIVIAL(debug) << "parser: size parsed sucessfully. value: " << size_;
                return true;
            } else {
                BOOST_LOG_TRIVIAL(error) << "parser error: wrong number of matches "
                          << "for size_header_expr pattern";
                return false; 
            }
        } else if(boost::regex_match(current_line, matches, indices_header_expr)) {
            size_t hits = matches.size();

            if(hits != 3) {
                BOOST_LOG_TRIVIAL(error) << "parser error: wrong number of matches for indices";
                return false;
            } else {
                std::string m(matches[1].first, matches[1].second);
                std::vector<std::string> indices;
                boost::split(indices, m, boost::is_any_of(" "));

                for(std::vector<std::string>::iterator it = indices.begin(); it < indices.end(); it++) {
                    int a = atoi((*it).c_str());

                    if(*it != "" && a >= 0) {
                        indices_.push_back(a);
                    } else {
                        BOOST_LOG_TRIVIAL(error) << "parser error: invalid index given";
                    }
                }
                
                std::stringstream res;
                for(std::vector<size_t>::iterator it = indices_.begin(); it != indices_.end(); ++it) {
                    res << *it << " ";
                }
                
                BOOST_LOG_TRIVIAL(debug) << "parser: indices parsed sucessfully. values: " << res.str();

                return true;
            }
        } else if(boost::regex_match(current_line, empty_line_expr)) {
            BOOST_LOG_TRIVIAL(debug) << "parser: sucessfully parsed empty line";
            return true;
        } else {
            return false;
        }
    } else {
        BOOST_LOG_TRIVIAL(error) << "parser error: undefined parser state";
        return false;
    }
}
