#ifndef ___REQ_PARSER___
#define ___REQ_PARSER___

#include <iostream>
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
