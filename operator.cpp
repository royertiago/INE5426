/* operator.cpp
 * Implementation of operator.h
 */

#include "operator.h"

std::ostream& NullaryOverload::print_to( std::ostream& os ) const {
    return os << "{NullaryOverload} " << name << " " << *body;
}
NullaryOverload * NullaryOverload::clone() const {
    return new NullaryOverload( name, body->clone() );
}

std::ostream& UnaryOverload::print_to( std::ostream& os ) const {
    return os << "{UnaryOverload} " << name << " " << *variable << " " << *body;
}
UnaryOverload * UnaryOverload::clone() const {
    return new UnaryOverload( name, body->clone(), variable->clone() );
}

std::ostream& BinaryOverload::print_to( std::ostream& os ) const {
    return os << "{UnaryOverload} " << name << " " << *left << " " << *right << " " << *body;
}
BinaryOverload * BinaryOverload::clone() const {
    return new BinaryOverload( name, body->clone(), left->clone(), right->clone() );
}
