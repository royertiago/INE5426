/* ast.cpp
 * Implementation of ast.h
 * The only functions implemented here are the "print_to", inherited
 * from Printable.
 */
#include "ast.h"

std::ostream& OperatorName::print_to( std::ostream& os ) const {
    return os << "{OpName}" << name;
}

std::ostream& NamedVariable::print_to( std::ostream& os ) const {
    return os << "{NamedVar} " << name;
}

std::ostream& RestrictedVariable::print_to( std::ostream& os ) const {
    return os << "{{RestrictedVar} " << name << '}';
}

std::ostream& NumberVariable::print_to( std::ostream& os ) const {
    return os << "{{NumberVar} " << name << '}';
}

std::ostream& PairVariable::print_to( std::ostream& os ) const {
    return os << "{{PairVar} " << *first << ", " << *second << '}';
}

std::ostream& PairBody::print_to( std::ostream& os ) const {
    return os << '{' << *first << ", " << *second << '}';
}

std::ostream& SequenceBody::print_to( std::ostream& os ) const {
    const char * separator = "";
    for( auto& e : sequence ) {
        os << separator << *e;
        separator = " ";
    }
    return os;
}

std::ostream& TerminalBody::print_to( std::ostream& os ) const {
    return os << name;
}

std::ostream& IncludeCommand::print_to( std::ostream& os ) const {
    return os << "include " << filename;
}

std::ostream& CategoryDefinition::print_to( std::ostream& os ) const {
    return os << "category " << name;
}

std::ostream& OperatorDefinition::print_to( std::ostream& os ) const {
    os << priority << '/' << format << '\n';
    const char * separator = "";
    for( auto& e : names ) {
        os << separator << *e;
        separator = " ";
    }
    os << '\n';
    os << *body;
    return os;
}
