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
    Symbol( std::string name ) : name( name ) {}
    virtual ~Symbol() = default;
    std::string name;
};

struct Category : public Symbol {
    Category( std::string name, unsigned value ) :
        Symbol( name ),
        value( value )
    {}
    unsigned value;

    /* Constructs a new category pointer.
     * Internal management is done to guarantee uniqueness
     * among the value of the category. */
    static std::unique_ptr<Category> next( std::string name );
};

#endif // SYMBOL_H
