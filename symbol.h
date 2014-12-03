/* symbol.h
 * Basic structure of the program.
 *
 * There are only four symbols: category,
 * and nullary, unary and binary operators.
 */
#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>

struct Symbol {
    virtual ~Symbol() = default;
    std::string name;
};

struct Category : public Symbol {
    unsigned value;
};

#endif // SYMBOL_H
