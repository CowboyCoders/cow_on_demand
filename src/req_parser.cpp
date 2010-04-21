#include <cstdlib> // for atio

#include "req_parser.hpp"

bool ReqParser::consume(char c)
{
    if(consumer_state == line)
    {
        if(c == '\r') 
        {
            parse(current_line);
            consumer_state = first_cr;
            return true;
        } 
        else if(c == '\n') 
        {
#ifdef DEBUG
            std::cerr << "parser error: newline character not expected" << std::endl;
#endif
            return false;
        } 
        else 
        {
            current_line.push_back(c);
            return true;
        }
    }
    else if(consumer_state == first_cr)
    {
        if(c == '\n')
        {
            consumer_state = first_nl;
            return true;
        }
        else
        {
#ifdef DEBUG
            std::cerr << "parser error: expected newline after carriage return" << std::endl;
#endif
            return false;
        }

    }
    else if(consumer_state == first_nl)
    {
        if(c == '\r')
        {
            consumer_state = second_cr;
        }
        else
        {
            // start of new line
            consumer_state = line;
            current_line.clear();
            current_line.push_back(c);
        }
        return true;
    }
    else if(consumer_state == second_cr)
    {
        if(c == '\n')
        {
            return true;
        }
        else
        {
#ifdef DEBUG
            std::cerr << "parser error: expected a terminating newline" << std::endl;
#endif
            return false;
        }
    }
    else
    {
#ifdef DEBUG
        std::cerr << "parser error: undefined consumer state" << std::endl;
#endif
        return false;
    }
}

bool ReqParser::parse(const std::string& line)
{
    if(parser_state == http_get)
    {
        boost::smatch matches;
        if(boost::regex_match(current_line, matches, http_get_expr))
        {
            if(matches.size() == 2)
            {
                file_ = matches[1];
                parser_state = headers;
                return true;
            }
            else
            {
#ifdef DEBUG
                std::cerr << "parser error: wrong number of matches "
                          << "for http_get_expr pattern" << std::endl;
#endif          
                return false; 
            }
        }
        else if(boost::regex_match(current_line, empty_line_expr))
        {
            return true;
        }
        else
        {
#ifdef DEBUG
            std::cerr << "parser error: malformed http get request"
                      << std::endl;
#endif
            return false;
        }
    }
    else if(parser_state == headers)
    {        
        boost::smatch matches;
        
        // Size parameter
        if(boost::regex_match(current_line, matches, size_header_expr))
        {
            if(matches.size() == 2)
            {
                std::string match = matches[1];
                size_ = atoi(match.c_str());
                return true;
            }
            else
            {
#ifdef DEBUG
                std::cerr << "parser error: wrong number of matches "
                          << "for size_header_expr pattern" << std::endl;
#endif          
                return false; 
            }
        }
        else if(boost::regex_match(current_line, matches, index_header_expr))
        {
            if(matches.size() == 2)
            {
                std::string match = matches[1];
                index_ = atoi(match.c_str());
                return true;
            }
            else
            {
#ifdef DEBUG
                std::cerr << "parser error: wrong number of matches "
                          << "for index_header_expr pattern" << std::endl;
#endif          
                return false; 
            }
        }        
        else if(boost::regex_match(current_line, matches, count_header_expr))
        {
            if(matches.size() == 2)
            {
                std::string match = matches[1];
                count_ = atoi(match.c_str());
                return true;
            }
            else
            {
#ifdef DEBUG
                std::cerr << "parser error: wrong number of matches "
                          << "for index_header_expr pattern" << std::endl;
#endif          
                return false; 
            }
        }
        else if(boost::regex_match(current_line, empty_line_expr))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
#ifdef DEBUG
        std::cerr << "parser error: undefined parser state" << std::endl;
#endif 
        return false;
    }
}
