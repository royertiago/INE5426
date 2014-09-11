/* lexer.h
 * Class that does the lexing phase of the compiler; it is a model of JavaIterator.
 */
#include <fstream>

#include <lexertl/generator.hpp>
#include <lexertl/lookup.hpp>
#include "position_iterator.hpp"
#include "token.h"

class Lexer {
    static lexertl::state_machine sm;
    static void init();

    std::string _data;
    lexertl::match_results<position_iterator<std::string::iterator>> _results;
    Token _next;
public:

    Lexer( const char * filename ) {
        init();
        std::ifstream in( filename, std::ios::in );
        if( !in ) {
            throw "Read error";
        }
        in.seekg( 0, std::ios::end );
        _data.resize( in.tellg() );
        in.seekg( 0, std::ios::beg );
        in.read( &_data[0], _data.size() );
        in.close();

        position_iterator<std::string::iterator> iter( _data.begin() );
        position_iterator<std::string::iterator> end( _data.end() );

        _results = lexertl::match_results<decltype(iter)>( iter, end );

        compute_next();
    }

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
    void compute_next() {
        lexertl::lookup(sm, _results);
        _next.id = _results.id;
        _next.lexeme = _results.str();
        _next.line = _results.start.line();
        _next.column = _results.start.column();
    }
};
