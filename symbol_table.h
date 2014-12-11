/* symbol_table.h
 * Class definition of the symbol table and declaration of the global symbol table.
 */
#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <set>
#include <string>
#include "symbol.h"
#include "operator.h"

/* Global symbol table used in the program.
 * The symbol table stores and retrieves the symbols defined in the program;
 * namely, categories and operators.
 */
namespace GlobalSymbolTable {
    void insertCategory( std::string name );
    bool existsCategory( std::string name );
    unsigned categoryValue( std::string name ); // assumes existsCategory

    /* Throws an exception if either
     *  - type is F and there is a category with same name, or
     *  - such an operator already exists with different priority. */
    void insertOverload( std::string name, std::string format, unsigned priority,
            std::unique_ptr<OperatorOverload>&& overload );

    bool existsBinaryOperator( std::string name );
    bool existsPrefixOperator( std::string name );
    bool existsPostfixOperator( std::string name );
    bool existsNullaryOperator( std::string name );
    bool existsOperator( std::string name );

    /* Returns the minimum priority a prefix/postfix/left/right
     * operand can have. This function takes account for the grouping
     * of operators, defined by it's types. */
    unsigned maximumPrefixPriority( std::string operator_name );
    unsigned maximumPostfixPriority( std::string operator_name );
    unsigned maximumLeftPriority( std::string operator_name );
    unsigned maximumRightPriority( std::string operator_name );

    /* Retrieves the priority of the operator.
     * assumes existsOperator*. */
    unsigned nullaryOperatorPriority( std::string operator_name );
    unsigned prefixOperatorPriority( std::string operator_name );
    unsigned postfixOperatorPriority( std::string operator_name );
    unsigned binaryOperatorPriority( std::string operator_name );

    /* Returns pointers to the requested operators,
     * or nullptr if no such operator exists in this file. */
    const NullaryOperator * retrieveNullaryOperator( std::string name );
    const UnaryOperator * retrievePrefixOperator( std::string name );
    const UnaryOperator * retrievePostfixOperator( std::string name );
    const BinaryOperator * retrieveBinaryOperator( std::string name );
}

/* Local symbol table used to store the operator variables. */
class LocalSymbolTable {
    std::set< std::string > table;

public:
    void insert( std::string symbol );
    bool contains( std::string symbol ) const;

    /* Joins two local symbol tables.
     * Returns *this. */
    LocalSymbolTable& merge( const LocalSymbolTable & );
};
#endif // SYMBOL_TABLE_H
