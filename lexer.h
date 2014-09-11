/* lexer.h
 * Class that does the lexing phase of the compiler; it is a model of JavaIterator.
 */
#include <string>
#include <lexertl/match_results.hpp>
#include "position_iterator.hpp"
#include "token.h"

class Lexer {
    std::string _data;
    lexertl::match_results<position_iterator<std::string::iterator>> _results;
    Token _next;
public:

    Lexer( const char * filename );

    auto next() {
        Token tmp = _next;
        compute_next();
        return tmp;
    }

    auto has_next() {
        return _next.id != 0;
    }

    auto peek() {
        return _next;
    }

private:
    void compute_next();
};
