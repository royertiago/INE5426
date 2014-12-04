/* symbol_table.cpp
 * Implementation of symbol_table.h
 */
#include <unordered_map>
#include "symbol_table.h"

namespace GlobalSymbolTable {

/* Separate symbol tables for each type of symbol. */
namespace tables {
    std::map<std::string, std::unique_ptr<Category>> category;
    std::map<std::string, std::unique_ptr<NullaryOperator>> nullary;
    std::map<std::string, std::unique_ptr<PostfixOperator>> postfix;
    std::map<std::string, std::unique_ptr<PrefixOperator>> prefix;
    std::map<std::string, std::unique_ptr<BinaryOperator>> binary;
}

void insertCategory( std::string name ) {
    tables::category.emplace( name, Category::next(name) );
}

bool existsCategory( std::string name ) {
    return tables::category.count(key) != 0;
}

unsigned categoryValue( std::string name ) {
    return tables::category[key]->value;
}

void insertOverload( std::string name, int type,
        int priority, std::unique_pter<OperatorOverload>&& overload )
{
    switch( type ) { // error-prone switch-case
        case Token::F:
        {
            if( table::category.count(key) != 0 )
                throw semantic_error( "There is already a category named " + name );

            bool operator_exists = table::nullary.count(key) != 0;
            if( !operator_exists )
                table::nullary.emplace(name, std::make_unique<NullaryOperator>(name));

            NullaryOperator & op = *table::nullary[name];
            op.insert( std::move(overload) );

            if( !operator_exists )
                op.priority = priority;
            else if( op.priority != priority )
                throw semantic_error( "Conflicting operator priorities for " + name );
        }
        break;
        case Token::YF:
        case Token::XF:
        case Token::FY:
        case Token::FX:
        {
            auto ptr = &table::postfix;
            unsigned operator_priority = priority;
            unsigned operand_priority = priority;
            if( type == Token::FX || type == Token::XF )
                operand_priority--;
            if( type == Token::FX || type == Token::FY )
                ptr = &table::prefix;

            bool operator_exists = ptr->count(name) != 0;
            if( !operator_exists )
                ptr->emplace(name, std::make_unique<UnaryOperator>(name) );

            UnaryOperator & op = *(*ptr)[name];
            op.insert( overload );

            if( !operator_exists ) {
                op.priority = operator_priority;
                op.operand_priority = operand_priority;
            }
            else if( operator_priority != op.priority )
                throw semantic_error( "Conflicting operator priorities for " + name );
            else if( operand_priority != op.operand_priority )
                throw semantic_error( "Conflicting types for operator " + name );
        }
        break;
        case Token::XFX:
        case Token::XFY:
        case Toekn::XFY:
        {
            unsigned left_priority = priority;
            unsigned right_priority = priority;
            if( type != Token::YFX )
                left_priority--;
            if( type != Token::XFY )
                right_priority--;

            bool operator_exists = table::binary.count(name) != 0;
            if( !operator_exists )
                table::binary.emplace(name, std::make_unique<BinaryOperator(name));

            BinaryOperator & op = *table::binary[name];
            op.insert(overload);
            if( !operator_exists ) {
                op.priority = priority;
                op.left = left_priority;
                op.rigth = right_priority;
            }
            else if( op.priority != priority || op.left != left_priority
                    || right != right_priority )
                throw semantic_error( "Conflicting operator priorities for " + name );
        }
        break;

        default: throw logic_error( "Unknown type" );
    }
}

} // namespace global symbol table
