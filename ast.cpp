/* ast.cpp
 * Implementation of ast.h
 * The only functions implemented here are the "print_to", inherited
 * from Printable.
 */
#include "ast.h"

std::ostream& OperatorName::print_to( std::ostream& os ) const {
    return os << "{OpName}" << name;
}
OperatorName * OperatorName::clone() const {
    return new OperatorName{ name };
}

std::ostream& NamedVariable::print_to( std::ostream& os ) const {
    return os << "{NamedVar} " << name;
}
NamedVariable * NamedVariable::clone() const {
    return new NamedVariable{ name };
}

std::ostream& RestrictedVariable::print_to( std::ostream& os ) const {
    return os << "{{RestrictedVar} " << name << '}';
}
RestrictedVariable * RestrictedVariable::clone() const {
    return new RestrictedVariable{ name };
}

std::ostream& NumericVariable::print_to( std::ostream& os ) const {
    return os << "{{NumberVar} " << name << '}';
}
NumericVariable * NumericVariable::clone() const {
    return new NumericVariable{ name, value };
}

std::ostream& PairVariable::print_to( std::ostream& os ) const {
    return os << "{{PairVar} " << *first << ", " << *second << '}';
}
PairVariable * PairVariable::clone() const {
    return new PairVariable{ first->clone(), second->clone() };
}

std::ostream& PairBody::print_to( std::ostream& os ) const {
    return os << '{' << *first << ", " << *second << '}';
}
PairBody * PairBody::clone() const {
    return new PairBody{ first->clone(), second->clone() };
}

std::ostream& SequenceBody::print_to( std::ostream& os ) const {
    const char * separator = "";
    for( auto& e : sequence ) {
        os << separator << *e;
        separator = " ";
    }
    return os;
}
SequenceBody * SequenceBody::clone() const {
    auto ret = new SequenceBody;
    for( auto& ptr : sequence )
        ret->sequence.emplace_back( ptr->clone() );
    return ret;
}

std::ostream& TerminalBody::print_to( std::ostream& os ) const {
    return os << name;
}
TerminalBody * TerminalBody::clone() const {
    return new TerminalBody{ name };
}

std::ostream& IncludeCommand::print_to( std::ostream& os ) const {
    return os << "include " << filename;
}
IncludeCommand * IncludeCommand::clone() const {
    return new IncludeCommand{ filename };
}

std::ostream& CategoryDefinition::print_to( std::ostream& os ) const {
    return os << "category " << name;
}
CategoryDefinition * CategoryDefinition::clone() const {
    return new CategoryDefinition{ name };
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
OperatorDefinition * OperatorDefinition::clone() const {
    OperatorDefinition * ret = new OperatorDefinition;
    ret->priority = priority;
    ret->format = format;
    ret->body.reset( body->clone() );
    for( auto& ptr : names )
        ret->names.emplace_back( ptr->clone() );
    return ret;
}
