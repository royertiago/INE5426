/* operator.cpp
 * Implementation of operator.h
 */

#include <ostream>
#include "operator.h"

std::ostream& NullaryOverload::print_to( std::ostream& os ) const {
    return os << "{{NullaryOverload} " << name << "\n" << *body << "\n}";
}
NullaryOverload * NullaryOverload::clone() const {
    return new NullaryOverload( name, body->clone() );
}
std::unique_ptr<Variable> NullaryOverload::compute() const {
    return body->evaluate( VariableTable() );
}

std::ostream& UnaryOverload::print_to( std::ostream& os ) const {
    return os << "{{UnaryOverload} " << name << "\n" << *variable << "\n" << *body << "\n}";
}
UnaryOverload * UnaryOverload::clone() const {
    return new UnaryOverload( name, body->clone(), variable->clone() );
}
std::unique_ptr<Variable> UnaryOverload::compute( std::unique_ptr<Variable>&& var ) const {
    VariableTable table;
    variable->decompose( *var, table );
    return body->evaluate( table );
}

std::ostream& BinaryOverload::print_to( std::ostream& os ) const {
    return os << "{{BinaryOverload} " << name << "\n" << *left << "\n"
                                    << *right << "\n" << *body << "\n}";
}
BinaryOverload * BinaryOverload::clone() const {
    return new BinaryOverload( name, body->clone(), left->clone(), right->clone() );
}
std::unique_ptr<Variable> BinaryOverload::compute(
        std::unique_ptr<Variable>&& left_var,
        std::unique_ptr<Variable>&& right_var
    ) const
{
    VariableTable table;
    left->decompose( *left_var, table );
    right->decompose( *right_var, table );
    return body->evaluate( table );
}
