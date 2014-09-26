/* ast.cpp
 * Implementation of ast.h
 * The only functions implemented here are the "print_to", inherited
 * from Printable.
 */
#include "ast.h"

std::ostream& GenericVariable::print_to( std::ostream& os ) const {
    return os << name;
}

std::ostream& NumericVariable::print_to( std::ostream& os ) const {
    return os << '{' << name << '}';
}

std::ostream& PairVariable::print_to( std::ostream& os ) const {
    return os << '{' << *first << ", " << *second << '}';
}

std::ostream& PairBody::print_to( std::ostream& os ) const {
    return os << '{' << *first << ", " << *second << '}';
}

std::ostream& SequenceBody::print_to( std::ostream& os ) const {
    const char * separator = "";
    for( auto& e : sequence ) {
        if( e.is<Token>() )
            os << separator << e.get<Token>();
        else
            os << separator << *e.get<std::unique_ptr<OperatorBody>>();
        separator = " ";
    }
    return os;
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
    for( auto& either : names ) {
        os << separator;
        if( either.is<Token>() )
            os << either.get<Token>();
        else
            os << *either.get< std::unique_ptr<OperatorVariable> >();
        separator = " ";
    }
    os << '\n';
    os << *body;
    return os;
}
