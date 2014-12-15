/* ast.cpp
 * Implementation of ast.h
 * The only functions implemented here are the "print_to", inherited
 * from Printable.
 */
#include <ostream>
#include <utility>
#include "ast.h"
#include "exceptions.h"
#include "operator.h"

// OperatorName
std::ostream& OperatorName::print_to( std::ostream& os ) const {
    return os << "{OpName} " << name;
}
OperatorName * OperatorName::clone() const {
    return new OperatorName{ name };
}

// NamedParameter
std::ostream& NamedParameter::print_to( std::ostream& os ) const {
    return os << "{NamedVar} " << name;
}
NamedParameter * NamedParameter::clone() const {
    return new NamedParameter{ name };
}
void NamedParameter::decompose( const Variable& var, VariableTable& table ) const {
    table.insert( name.lexeme, std::move(var.clone()) );
}

// RestrictedParameter
std::ostream& RestrictedParameter::print_to( std::ostream& os ) const {
    return os << "{{RestrictedVar} " << name << '}';
}
RestrictedParameter * RestrictedParameter::clone() const {
    return new RestrictedParameter{ name };
}
void RestrictedParameter::decompose( const Variable& var, VariableTable& table ) const {
    if( !var.first ) throw semantic_error( name.lexeme + " needs to be a number" );
    table.insert( name.lexeme, std::move(var.clone()) );
}

// NumericParameter
std::ostream& NumericParameter::print_to( std::ostream& os ) const {
    return os << "{{NumberVar} " << name << '}';
}
NumericParameter * NumericParameter::clone() const {
    return new NumericParameter{ name, value };
}
void NumericParameter::decompose( const Variable& var, VariableTable& ) const {
    if( var.first ) throw semantic_error( name.lexeme + " needs to be a number" );
    if( var.value != value ) throw semantic_error( "Unmatched value" );
    // NumericParameter is a mere matching Parameter.
}

// PairParameter
std::ostream& PairParameter::print_to( std::ostream& os ) const {
    return os << "{{PairVar} " << *first << ", " << *second << '}';
}
PairParameter * PairParameter::clone() const {
    return new PairParameter{ first->clone(), second->clone() };
}
void PairParameter::decompose( const Variable& var, VariableTable& table ) const {
    first->decompose( var, table );
    second->decompose( var, table );
}

// PairBody
std::ostream& PairBody::print_to( std::ostream& os ) const {
    return os << "{{PairBody} " << *first << ", " << *second << '}';
}
PairBody * PairBody::clone() const {
    return new PairBody{ first->clone(), second->clone() };
}
std::unique_ptr<Variable> PairBody::evaluate( const VariableTable& table ) const {
    auto lptr = std::move( first->evaluate(table) );
    auto rptr = std::move( second->evaluate(table) );
    return std::make_unique<Variable>( std::move(lptr), std::move(rptr) );
    // step-by-step evaluation guarantees left-to-right evaluation.
}

// SequenceBody
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
std::unique_ptr<Variable> SequenceBody::evaluate( const VariableTable& ) const {
    throw std::logic_error( "SequenceBody::evaluate called" );
}

// TerminalBody
std::ostream& TerminalBody::print_to( std::ostream& os ) const {
    return os << "{TerminalBody} " << name;
}
TerminalBody * TerminalBody::clone() const {
    return new TerminalBody{ name };
}
std::unique_ptr<Variable> TerminalBody::evaluate( const VariableTable& ) const {
    throw std::logic_error( "TerminalBody::evaluate called" );
}

// VariableBody
std::ostream& VariableBody::print_to( std::ostream& os ) const {
    return os << "{VariableBody} " << name;
}
VariableBody * VariableBody::clone() const {
    return new VariableBody{ name };
}
std::unique_ptr<Variable> VariableBody::evaluate( const VariableTable& table ) const {
    return table.retrieve(name);
}

// NumericBody
std::ostream& NumericBody::print_to( std::ostream& os ) const {
    return os << "{NumberBody} " << value;
}
NumericBody * NumericBody::clone() const {
    return new NumericBody{ value };
}
std::unique_ptr<Variable> NumericBody::evaluate( const VariableTable& ) const {
    return std::make_unique<Variable>( value );
}

// NullaryTreeBody
std::ostream& NullaryTreeBody::print_to( std::ostream& os ) const {
    return os << "{NullaryTreeBody} " << op->name;
}
NullaryTreeBody * NullaryTreeBody::clone() const {
    return new NullaryTreeBody{ op }; // note there is no 'clone'
}
std::unique_ptr<Variable> NullaryTreeBody::evaluate( const VariableTable& ) const {
    return op->compute();
}

// UnaryTreeBody
std::ostream& UnaryTreeBody::print_to( std::ostream& os ) const {
    return os << "{UnaryTreeBody} " << op->name << " " << *variable;
}
UnaryTreeBody * UnaryTreeBody::clone() const {
    return new UnaryTreeBody{ op, variable->clone() };
}
std::unique_ptr<Variable> UnaryTreeBody::evaluate( const VariableTable& table ) const {
    return op->compute( variable->evaluate( table ) );
}

// BinaryTreeBody
std::ostream& BinaryTreeBody::print_to( std::ostream& os ) const {
    return os << "{{BinaryTreeBody} " << *left << " " << op->name << " " << *right << " }";
}
BinaryTreeBody * BinaryTreeBody::clone() const {
    return new BinaryTreeBody{ op, left->clone(), right->clone() };
}
std::unique_ptr<Variable> BinaryTreeBody::evaluate( const VariableTable& table ) const {
    auto lptr = std::move( left->evaluate(table) );
    auto rptr = std::move( right->evaluate(table) );
    return op->compute( std::move(lptr), std::move(rptr) );
}

// IncludeCommand
std::ostream& IncludeCommand::print_to( std::ostream& os ) const {
    return os << "{Include} " << filename;
}
IncludeCommand * IncludeCommand::clone() const {
    return new IncludeCommand{ filename };
}

// CategoryDefinition
std::ostream& CategoryDefinition::print_to( std::ostream& os ) const {
    return os << "{Category} " << name;
}
CategoryDefinition * CategoryDefinition::clone() const {
    return new CategoryDefinition{ name };
}

// OperatorDefinition
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
