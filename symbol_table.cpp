/* symbol_table.cpp
 * Implementation of symbol_table.h
 */
#include <unordered_map>
#include "exceptions.h"
#include "symbol_table.h"

namespace GlobalSymbolTable {

/* Separate symbol tables for each type of symbol. */
namespace tables {
    std::unordered_map<std::string, std::unique_ptr<Category>> category;
    std::unordered_map<std::string, std::unique_ptr<NullaryOperator>> nullary;
    std::unordered_map<std::string, std::unique_ptr<PostfixOperator>> postfix;
    std::unordered_map<std::string, std::unique_ptr<ProfixOperator>> prefix;
    std::unordered_map<std::string, std::unique_ptr<BinaryOperator>> binary;
} // namespace tables

void insertCategory( std::string name ) {
    tables::category.emplace( name, Category::next(name) );
}

bool existsCategory( std::string name ) {
    return tables::category.count(key) != 0;
}

unsigned categoryValue( std::string name ) {
    return tables::category[key]->value;
}

void insertOverload( std::string name, std::string format,
        unsigned priority, std::unique_ptr<OperatorOverload>&& overload )
 {
    if( format == "F" ) {
        if( tables::category.count(name) != 0 )
            throw semantic_error( "There is already a category named " + name );

        bool operator_exists = tables::nullary.count(name) != 0;
        if( !operator_exists )
            tables::nullary.emplace(name, std::make_unique<NullaryOperator>(name));

        NullaryOperator & op = *tables::nullary[name];
        op.insert( std::move(overload) );

        if( !operator_exists )
            op.priority = priority;
        else if( op.priority != priority )
            throw semantic_error( "Conflicting operator priorities for " + name );
    }
    else if( format == "YF" || format == "XF" || format == "FY" || format == "FX" ) {
        auto ptr = &tables::postfix;
        unsigned operator_priority = priority;
        unsigned operand_priority = priority;
        if( format == "FX" || format == "XF" )
            operand_priority--;
        if( format == "FX" || format == "FY" )
            ptr = &tables::prefix;

        bool operator_exists = ptr->count(name) != 0;
        if( !operator_exists )
            ptr->emplace(name, std::make_unique<UnaryOperator>(name) );

        UnaryOperator & op = *(*ptr)[name];
        op.insert( std::move(overload) );

        if( !operator_exists ) {
            op.priority = operator_priority;
            op.operand_priority = operand_priority;
        }
        else if( operator_priority != op.priority )
            throw semantic_error( "Conflicting operator priorities for " + name );
        else if( operand_priority != op.operand_priority )
            throw semantic_error( "Conflicting types for operator " + name );
    }
    else if( format == "XFX" || format == "XFY" || format == "YFX" ) {
        unsigned left_priority = priority;
        unsigned right_priority = priority;
        if( format != "YFX" )
            left_priority--;
        if( format != "XFY" )
            right_priority--;

        bool operator_exists = tables::binary.count(name) != 0;
        if( !operator_exists )
            tables::binary.emplace(name, std::make_unique<BinaryOperator>(name));

        BinaryOperator & op = *tables::binary[name];
        op.insert( std::move(overload) );
        if( !operator_exists ) {
            op.priority = priority;
            op.left_priority = left_priority;
            op.right_priority = right_priority;
        }
        else if( op.priority != priority || op.left_priority != left_priority
                || op.right_priority != right_priority )
            throw semantic_error( "Conflicting operator priorities for " + name );
    }
    else
        throw std::logic_error( "Unknown type" );
}

bool existsBinaryOperator( std::string name ) {
    return retrieveBinaryOperator( name ) == nullptr;
}
bool existsPrefixOperator( std::string name ) {
    return retrievePrefixOperator( name ) == nullptr;
}
bool existsPostfixOperator( std::string name ) {
    return retrievePostfixOperator( name ) == nullptr;
}
bool existsNullaryOperator( std::string name ) {
    return retrieveNullaryOperator( name ) == nullptr;
}
bool existsOperator( std::string name ) {
    return existsNullaryOperator(name) ||
           existsPostfixOperator(name) ||
           existsPrefixOperator(name) ||
           existsBinaryOperator(name);
}

int maximumPrefixPriority( std::string operator_name ) {
    return table::prefix[name].operand_priority;
}
int maximumPostfixPriority( std::string operator_name ) {
    return table::postfix[name].operand_priority;
}
int maximumLeftPriority( std::string operator_name ) {
    return table::binary[name].left;
}
int maximumRightPriority( std::string operator_name ) {
    return table::binary[name].right;
}

int nullaryOperatorPriority( std::string operator_name ) {
    return table::nullary[name].priority;
}
int prefixOperatorPriority( std::string operator_name ) {
    return table::prefix[name].priority;
}
int postfixOperatorPriority( std::string operator_name ) {
    return table::postfix[name].priority;
}
int binaryOperatorPriority( std::string operator_name ) {
    return table::binary[name].priority;
}

const NullaryOperator * retrieveNullaryOperator( std::string name );
    auto iter = table::nullary.find( name );
    if( iter == table::nullary.end() )
        return nullptr;
    return iter->second.get();
}
const UnaryOperator * retrievePrefixOperator( std::string name ) {
    auto iter = table::prefix.find( name );
    if( iter == table::prefix.end() )
        return nullptr;
    return iter->second.get();
}
const UnaryOperator * retrievePostfixOperator( std::string name ) {
    auto iter = table::postfix.find( name );
    if( iter == table::postfix.end() )
        return nullptr;
    return iter->second.get();
}
const BinaryOperator * retrieveBinaryOperator( std::string name ) {
    auto iter = table::binary.find( name );
    if( iter == table::binary.end() )
        return nullptr;
    return iter->second.get();
}

} // namespace global symbol table

// Implementation of LocalSymbolTable methods.
void LocalSymbolTable::insert( std::string name ) {
    table.insert( name );
}

bool LocalSymbolTable::contains( std::string name ) const {
    return table.count( name ) != 0;
}
