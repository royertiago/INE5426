/* exceptions.h
 * List of all exceptions thrown by the program.
 */
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include "token.h"

struct file_error : public std::runtime_error {
    file_error( const char * what ) :
        runtime_error( what )
    {}
    file_error( std::string & what ) :
        runtime_error( what )
    {}
};

struct parse_error : public std::runtime_error {
    Token where;
    parse_error( const char * what, Token where ) :
        runtime_error( what ),
        where( where )
    {}
    parse_error( std::string & what, Token where ) :
        runtime_error( what ),
        where( where )
    {}
};

struct semantic_error : public std::runtime_error {
    semantic_error( const char * what ) :
        runtime_error( what )
    {}
    semantic_error( std::string & what ) :
        runtime_error( what )
    {}
};
#endif // EXCEPTIONS_H
