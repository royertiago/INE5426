/* ast.h
 * Structures that composes the abstract syntax tree.
 */
#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>
#include "printable.h"
#include "utility/either.hpp"
#include "token.h"

/* Operator variable is the structure seen in the operator
 * definitions that defines the structure of that overload.
 *
 * Example:
 *  xf 800 {X, {Y}} ++
 *
 * The OperatorVariable is identified by {X, {Y}}. The outer
 * variable is an PairVariable. Its first value is an
 * GenericVariable, whose token refers to 'X'; the second
 * value of the pair is a NumericVariable, whose token is 'Y'.
 *
 * Tuples like {X, Y, Z, W} are a shorthand to {X, {Y, {Z, W}}}.
 */
struct OperatorVariable : public Printable {
    virtual ~OperatorVariable() = default;
};

struct GenericVariable : public OperatorVariable {
    Token name;
    virtual std::ostream& print_to( std::ostream& ) const override;
};

struct NumericVariable : public OperatorVariable {
    Token name;
    virtual std::ostream& print_to( std::ostream& ) const override;
};

struct PairVariable : public OperatorVariable {
    std::unique_ptr<OperatorVariable> first;
    std::unique_ptr<OperatorVariable> second;
    virtual std::ostream& print_to( std::ostream& ) const override;
};

/* Operator bodies.
 * A PairBody is used in the same way as a PairVariable, but there
 * is no need to use braces. Braces are used to force priority; thus
 * the need for separate SequencedBody from TerminalBody. */
struct OperatorBody : public Printable {
    virtual ~OperatorBody() = default;
};

struct PairBody : public OperatorBody {
    std::unique_ptr<OperatorBody> first;
    std::unique_ptr<OperatorBody> second;
    virtual std::ostream& print_to( std::ostream& ) const override;
};

struct SequencedBody : public OperatorBody {
    std::vector<std::unique_ptr<OperatorBody>> sequence;
    virtual std::ostream& print_to( std::ostream& ) const override;
};

struct TerminalBody : public OperatorBody {
    Token value;
    virtual std::ostream& print_to( std::ostream& ) const override;
};

/* A statement is the basic structure of the program.
 * There are three statements: IncludeCommand, CategoryDefinition
 * and OperatorDefinition. */
struct Statement : public Printable {
    virtual ~Statement() = default;
};

struct IncludeCommand : public Statement {
    Token filename;
    virtual std::ostream& print_to( std::ostream& ) const override;
};

struct CategoryDefinition : public Statement {
    Token name;
    virtual std::ostream& print_to( std::ostream& ) const override;
};

struct OperatorDefinition : public Statement {
    unsigned priority;
    std::string format;
    std::vector< either<std::unique_ptr<OperatorVariable>, Token> > names;
    std::unique_ptr<OperatorBody> body;
    virtual std::ostream& print_to( std::ostream& ) const override;
};

#endif // AST_H
