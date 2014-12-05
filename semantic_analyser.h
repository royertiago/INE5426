/* semantic_analyser.h
 * Object that does the semantical analysis of the program.
 *
 * SemanticAnalysers encapsulates a Parser and walk through
 * every semantic structure, calling tree_build.h functions
 * as needed, processing include commands, and populating
 * the GlobalSymbolTable in the process.
 *
 * There should be at most one SemanticAnalyser per program,
 * as these objects modify the GlobalSymbolTable.
 */
#ifndef SEMANTIC_ANALYSER_H
#define SEMANTIC_ANALYSER_H

#include "parser.h"

struct SemanticAnalyser {
    void set_parser( std::unique_ptr<Parser>&& );

    std::unique_ptr<Statement> next();

    /* The returned pointer should not be deleted. */
    const Statement * peek();

    /* Determines if there is more tokens to be parsed.
     * The functions next() and peek() shall only be
     * called if has_next() returns true. */
    bool has_next() const;

private:
    std::unique_ptr<Parser>&& _current_parser
    std::unique_ptr<Statement> _next;
    void compute_next();
};


#endif // SEMANTIC_ANALYSER_H
