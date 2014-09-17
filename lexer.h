/* lexer.h
 * Class that does the lexing phase of the compiler; it is a model of JavaIterator.
 *
 * Although the lexing is done on demand, all the contents of the file are
 * read to a std::string on construction. This string is shared by
 * all the copies of the lexer.
 */
#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <memory>
#include <lexertl/match_results.hpp>
#include "position_iterator.hpp"
#include "token.h"
#include "exceptions.h"

struct Lexer {
    /* Constructs the lexer to scan the contents of the specified file.
     * The lexer should start "pointing" to the first token, that can be 
     * readily retrieved via peek() or via next(); if this does not
     * happens, has_next() returns false (and conversely).
     *
     * If the file is not readable by some reason, file_error is thrown.
     */
    Lexer( const char * filename );

    /* Reads the current token and advances the state of the lexer.
     * Can only be executed if has_next() returns true. */
    auto next() {
        Token tmp = _next;
        compute_next();
        return tmp;
    }

    /* True if there are at least one more token to be read. */
    auto has_next() {
        return _next.id != 0;
    }

    /* Reads the current token, without advancing the state of
     * the lexer.
     * Can only be executed if has_next() returns true. */
    auto peek() {
        return _next;
    }

private:
    std::shared_ptr<std::string> _file;
    lexertl::match_results<position_iterator<std::string::iterator>> _results;
    Token _next;

    /* Computes the next token in the file and stores in Lexer::_next. */
    void compute_next();
};

#endif // LEXER_H
