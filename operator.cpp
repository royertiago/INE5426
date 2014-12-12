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

std::ostream& UnaryOverload::print_to( std::ostream& os ) const {
    return os << "{{UnaryOverload} " << name << "\n" << *variable << "\n" << *body << "\n}";
}
UnaryOverload * UnaryOverload::clone() const {
    return new UnaryOverload( name, body->clone(), variable->clone() );
}

std::ostream& BinaryOverload::print_to( std::ostream& os ) const {
    return os << "{{BinaryOverload} " << name << "\n" << *left << "\n"
                                    << *right << "\n" << *body << "\n}";
}
BinaryOverload * BinaryOverload::clone() const {
    return new BinaryOverload( name, body->clone(), left->clone(), right->clone() );
}
