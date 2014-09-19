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
    {
        compute_next();
    }

    std::unique_ptr<Statement> next() {
        auto tmp = std::move(_next);
        compute_next();
        return std::move(tmp);
    }

    /* The returned pointer should not be deleted. */
    const Statement * peek() const {
        return _next.get();
    }

    bool has_next() const {
        return _next.operator bool();
    }

private:
    Lexer _alex;
    std::unique_ptr<Statement> _next;
    void compute_next();
};

#endif // PARSER_H
