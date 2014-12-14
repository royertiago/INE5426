/* variable.cpp
 * Implementation of variable.h
 */
#include <utility>
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
