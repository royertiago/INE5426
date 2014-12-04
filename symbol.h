/* symbol.h
 * Basic structure of the program.
 *
 * There are only four symbols: category,
 * and nullary, unary and binary operators.
 */
#ifndef SYMBOL_H
#define SYMBOL_H

#include <memory>
#include <string>

struct Symbol {
    virtual ~Symbol() = default;
    std::string name;
};

struct Category : public Symbol {
    unsigned value;

    /* Constructs a new category pointer.
     * Internal management is done to guarantee uniqueness
     * among the value of the category. */
    static std::unique_ptr<Category> next( std::string name );
};

#endif // SYMBOL_H
