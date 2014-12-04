/* symbol_table.h
 * Class definition of the symbol table and declaration of the global symbol table.
 */
#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include "symbol.h"
#include "operator.h"

/* Global symbol table used in the program.
 * The symbol table stores and retrieves the symbols defined in the program;
 * namely, categories and operators.
 */
class GlobalSymbolTable {
    std::unordered_map<std::string, Symbol> table;

    GlobalSymbolTable() = default;
public:
    /* The only instance of the global symbol table. */
    static GlobalSymbolTable instance;


    void insertCategory( std::string name );
    bool existsCategory( std::string name ) const;
    int categoryValue( std::string name ) const;

    /* Throws an exception if either
     *  - type is F and there is a category with same name, or
     *  - such an operator already exists with different priority. */
    void insertOverload( std::string name, int type, int priority,
            OperatorOverload overload );

    bool existsBinaryOperator( std::string name ) const;
    bool existsPrefixOperator( std::string name ) const;
    bool existsPostfixOperator( std::string name ) const;
    bool existsNullaryOperator( std::string name ) const;
    bool existsOperator( std::string name ) const;

    /* Returns the minimum priority a prefix/postfix/left/right
     * operand can have. This function takes account for the grouping
     * of operators, defined by it's types. */
    int maximumPrefixPriority( std::string operator_name ) const;
    int maximumPostfixPriority( std::string operator_name ) const;
    int maximumLeftPriority( std::string operator_name ) const;
    int maximumRightPriority( std::string operator_name ) const;

    /* Retrieves the priority of the operator.
     * assumes existsOperator*. */
    int nullaryOperatorPriority( std::string operator_name );
    int prefixOperatorPriority( std::string operator_name );
    int postfixOperatorPriority( std::string operator_name );
    int binaryOperatorPriority( std::string operator_name );

    /* Returns pointers to the requested operators,
     * or nullptr if no such operator exists in this file. */
    const NullaryOperator * retrieveNullaryOperator( std::string name ) const;
    const UnaryOperator * retrievePrefixOperator( std::string name ) const;
    const UnaryOperator * retrievePostfixOperator( std::string name ) const;
    const BinaryOperator * retrieveBinaryOperator( std::string name ) const;
};

/* Local symbol table used to store the operator variables. */
class LocalSymbolTable {
    std::set< std::string > table;

public:
    void insert( std::string symbol );
    bool contains( std::string symbol ) const;
};
#endif // SYMBOL_TABLE_H
