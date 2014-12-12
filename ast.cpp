/* ast.cpp
 * Implementation of ast.h
 * The only functions implemented here are the "print_to", inherited
 * from Printable.
 */
#include <ostream>
#include "ast.h"
#include "operator.h"

std::ostream& OperatorName::print_to( std::ostream& os ) const {
    return os << "{OpName} " << name;
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
    return os << "{{PairBody} " << *first << ", " << *second << '}';
}
PairBody * PairBody::clone() const {
    return new PairBody{ first->clone(), second->clone() };
}

std::ostream& SequenceBody::print_to( std::ostream& os ) const {
    os << "{SequenceBody}";
    for( auto& e : sequence )
        os << " " << *e;
    return os;
}
SequenceBody * SequenceBody::clone() const {
    auto ret = new SequenceBody;
    for( auto& ptr : sequence )
        ret->sequence.emplace_back( ptr->clone() );
    return ret;
}

std::ostream& TerminalBody::print_to( std::ostream& os ) const {
    return os << "{TerminalBody} " << name;
}
TerminalBody * TerminalBody::clone() const {
    return new TerminalBody{ name };
}

std::ostream& VariableBody::print_to( std::ostream& os ) const {
    return os << "{VariableBody} " << name;
}
VariableBody * VariableBody::clone() const {
    return new VariableBody{ name };
}

std::ostream& NumericBody::print_to( std::ostream& os ) const {
    return os << "{NumberBody} " << value;
}
NumericBody * NumericBody::clone() const {
    return new NumericBody{ value };
}

std::ostream& NullaryTreeBody::print_to( std::ostream& os ) const {
    return os << "{NullaryTreeBody} " << op->name;
}
NullaryTreeBody * NullaryTreeBody::clone() const {
    return new NullaryTreeBody{ op }; // note there is no 'clone'
}

std::ostream& UnaryTreeBody::print_to( std::ostream& os ) const {
    return os << "{UnaryTreeBody} " << op->name << " " << *variable;
}
UnaryTreeBody * UnaryTreeBody::clone() const {
    return new UnaryTreeBody{ op, variable->clone() };
}

std::ostream& BinaryTreeBody::print_to( std::ostream& os ) const {
    return os << "{{BinaryTreeBody} " << *left << " " << op->name << " " << *right << " }";
}
BinaryTreeBody * BinaryTreeBody::clone() const {
    return new BinaryTreeBody{ op, left->clone(), right->clone() };
}

std::ostream& IncludeCommand::print_to( std::ostream& os ) const {
    return os << "{Include} " << filename;
}
IncludeCommand * IncludeCommand::clone() const {
    return new IncludeCommand{ filename };
}

std::ostream& CategoryDefinition::print_to( std::ostream& os ) const {
    return os << "{Category} " << name;
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
