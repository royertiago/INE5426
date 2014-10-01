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
    Parser( const char * filename );

    /* The unique pointer ownership is transferred. */
    std::unique_ptr<Statement> next();

    /* The returned pointer should not be deleted. */
    const Statement * peek();

    /* Determines if there is more tokens to be parsed.
     * The functions next() and peek() shall only be
     * called if has_next() returns true. */
    bool has_next() const;

    /* Error-recovering mode. */
    void panic();

private:
    Lexer _alex;
    std::unique_ptr<Statement> _next;
    void compute_next();
};

#endif // PARSER_H
