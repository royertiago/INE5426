/* parser.h
 * Class that does most of syntatical analysis and
 * abstract syntax tree (AST) construction.
 *
 * This class does "most" of the analysis due to the possibility
 * of creating operators in the language, so building the syntax
 * tree of non-parenthesized expressions must be done at semantic
 * analysis.
 */
#ifndef PARSER_H
#define PARSER_H

#include <utility>
#include "lexer.h"
#include "ast.h"

struct Parser {
    Parser( const char * filename ) :
        _alex( filename ),
        _next( nullptr )
    {}

    std::unique_ptr<Statement> next() {
        if( !_next && _alex.has_next() )
            compute_next();

        auto tmp = std::move(_next);
        compute_next();
        return std::move(tmp);
    }

    /* The returned pointer should not be deleted. */
    const Statement * peek() {
        if( !_next )
            compute_next();
        return _next.get();
    }

    bool has_next() const {
        return _alex.has_next();
    }

    /* Error-recovering mode. */
    void panic() {
        while( _alex.has_next() && !Token::declarator(_alex.peek()) )
            _alex.next();
    }

private:
    Lexer _alex;
    std::unique_ptr<Statement> _next;
    void compute_next();
};

#endif // PARSER_H
