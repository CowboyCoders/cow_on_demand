#include <cstdlib> // for atio
#include <boost/algorithm/string.hpp> // for boost::split
#include <boost/log/trivial.hpp>

#include "req_parser.hpp"

/**
 * Parser state management.
 */
bool ReqParser::consume(char c)
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
bool ReqParser::parse(const std::string& line)
{
    if(parser_state == http_get) {
        boost::smatch matches;

        if(boost::regex_match(current_line, matches, http_get_expr)) {
            if(matches.size() == 2) {
                file_ = matches[1];
                parser_state = headers;
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

                return true;
            }
            /*
             * TODO: REMOVE THIS
             * BOOST_LOG_TRIVIAL(fatal) << "HITS: " << hits;
             * for(int i = 1; i < hits; i++) {
             *    std::string m(matches[i].first, matches[i].second);
             *    BOOST_LOG_TRIVIAL(fatal) << "HIT: " << matches[i];
             * }
             */

        /*
        } else if(boost::regex_match(current_line, matches, index_header_expr)) {
            if(matches.size() == 2) {
                std::string match = matches[1];
                index_ = atoi(match.c_str());
                return true;
            } else {
                BOOST_LOG_TRIVIAL(error) << "parser error: wrong number of matches for index_header_expr pattern";
                return false; 
            }
        } else if(boost::regex_match(current_line, matches, count_header_expr)) {
            if(matches.size() == 2) {
                std::string match = matches[1];
                count_ = atoi(match.c_str());

                return true;
            } else {
            BOOST_LOG_TRIVIAL(error) << "parser error: wrong number of matches "
                          << "for index_header_expr pattern";
                return false; 
            }
        */
        } else if(boost::regex_match(current_line, empty_line_expr)) {
            return true;
        } else {
            return false;
        }
    } else {
        BOOST_LOG_TRIVIAL(error) << "parser error: undefined parser state";
        return false;
    }
}
