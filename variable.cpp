/* variable.cpp
 * Implementation of variable.h
 */
#include <ostream>
#include <utility>
#include "exceptions.h"
#include "variable.h"

Variable::Variable( std::unique_ptr<Variable>&& f, std::unique_ptr<Variable>&& s ) :
    first( std::move(f) ),
    second( std::move(s) )
{}

Variable::Variable( long long value ) :
    first( nullptr ),
    value( value )
{}

Variable::~Variable() {
    if( first ) second.reset();
}

std::unique_ptr<Variable> Variable::clone() const {
    if( !first ) return std::make_unique<Variable>( value );
    return std::make_unique<Variable>(
            std::move( first->clone() ),
            std::move( second->clone() )
        );
}

bool operator==( const Variable& lhs, const Variable& rhs ) {
    if( lhs.first )
        return rhs.first &&
                *lhs.first == *rhs.first &&
                *lhs.second == *rhs.second;
    return !rhs.first &&
        lhs.value == rhs.value;
}

std::ostream& operator<<( std::ostream & os, const Variable& var ) {
    if( var.first ) return os << '{' << *var.first << ", " << *var.second << '}';
    return os << var.value;
}

void VariableTable::insert( std::string name, std::unique_ptr<Variable>&& variable ) {
    auto pair = table.insert( std::make_pair(name, variable->clone()) );
    if( pair.second ) {
        // Insertion failed: this variable exists.
        if( *pair.first->second == *variable )
            return; // Ok: the values are the same.
        throw semantic_error( "Variable already inserted with different value" );
    }
}

std::unique_ptr<Variable> VariableTable::retrieve( std::string name ) const {
    try {
        return table.at(name)->clone();
    } catch( std::out_of_range & ) {
        throw semantic_error( "Inexistent variable" );
    }
}
