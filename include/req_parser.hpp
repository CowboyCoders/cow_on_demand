#ifndef ___REQ_PARSER___
#define ___REQ_PARSER___

#include <iostream>
#include <string>

#include <boost/regex.hpp>
#include <boost/tokenizer.hpp>

class ReqParser {
public:
    ReqParser()
        : size_(0), 
          index_(0), 
          count_(0),
          indices_(),
          consumer_state(line),
          parser_state(http_get),
          // regular expressions
          http_get_expr("GET /(\\S+) HTTP/1.(?:0|1)"),
          size_header_expr("[Ss]ize: ?([0-9]+)"),
          index_header_expr("[Ii]ndex: ?([0-9]+)"),
          count_header_expr("[Cc]ount: ?([0-9]+)"),
          indices_header_expr("[Ii]ndices: (([0-9]+[ ]{0,1})+)"),
          empty_line_expr("^\\s*$")
    {
    }

    bool consume(char c);

    std::vector<size_t> indices() const
    {
        return indices_;
    }

    std::string file() const
    {
        return file_;
    }

    size_t size() const
    {
        return size_;
    }

    size_t index() const
    {
        return index_;
    }

    size_t count() const
    {
        return count_;
    }

private:
    bool parse(const std::string& line); 

    std::string file_;
    size_t size_;
    size_t index_;
    size_t count_;
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
