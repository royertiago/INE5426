/* ast.h
 * Structures that composes the abstract syntax tree.
 */
#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>
#include "operator_fwd.h"
#include "printable.h"
#include "token.h"

#define AUX_FORWARD(var) std::forward<decltype(var)>(var)

/* SignatureToken is either an OperatorVariable or an OperatorName.
 * Ex:
 * xfx 800 Q ** P
 * Q and P are OperatorVariables and ** is an OperatorName. */
struct SignatureToken : public Printable {
    virtual ~SignatureToken() = default;
};

struct OperatorName : public SignatureToken {
    OperatorName() = default;
    OperatorName( auto&& t ) : name(AUX_FORWARD(t)) {}
    Token name;
    virtual std::ostream& print_to( std::ostream& ) const override;
};

/* OperatorVariable is the structure seen in the operator
 * definitions that defines the structure of that overload.
 *
 * Example:
 *  xf 800 {X, {Y}} ++
 *
 * The OperatorVariable is identified by {X, {Y}}. The outer
 * variable is an PairVariable. Its first value is an
 * GenericVariable, whose token refers to 'X'; the second
 * value of the pair is a RestrictedVariable, whose token is 'Y'.
 *
 * Tuples like {X, Y, Z, W} are a shorthand to {X, {Y, {Z, W}}}.
 *
 * In operator signatures, is also possible to appear numbers,
 * as a way to restrict the input ("value overload"). For example,
 *
 *  xf 300 0 !
 *      1
 *
 * This code defines 0 ! as 1. These variables are represented
 * via class NumberVariable.
 */
struct OperatorVariable : public SignatureToken {
    virtual ~OperatorVariable() = default;
};

struct NamedVariable : public OperatorVariable {
    NamedVariable() = default;
    NamedVariable( auto&& t ) : name(AUX_FORWARD(t)) {}
    Token name;
    virtual std::ostream& print_to( std::ostream& ) const override;
};

struct RestrictedVariable : public OperatorVariable {
    RestrictedVariable() = default;
    RestrictedVariable( auto&& t ) : name(AUX_FORWARD(t)) {}
    Token name;
    virtual std::ostream& print_to( std::ostream& ) const override;
};

struct NumberVariable : public OperatorVariable {
    NumberVariable() = default;
    NumberVariable( auto&& t ) : name(AUX_FORWARD(t)) {}
    Token name;
    virtual std::ostream& print_to( std::ostream& ) const override;
};

struct PairVariable : public OperatorVariable {
    PairVariable() = default;
    PairVariable(auto&& first, auto&& second) :
        first(AUX_FORWARD(first)),
        second(AUX_FORWARD(second))
    {}
    std::unique_ptr<OperatorVariable> first;
    std::unique_ptr<OperatorVariable> second;
    virtual std::ostream& print_to( std::ostream& ) const override;
};

/* Operator bodies.
 * A PairBody is used in the same way as a PairVariable, but there
 * is no need to use braces. Braces are used to force priority.
 * SequenceBody is used to aggregate sequential expressions.
 * To represent nesting, an element in the sequence can mean
 * either a sequence Token (Token::sequence) or a nested operator
 * body.
 *
 * SequenceBody and TerminalBody are used in the parsing only; semantical
 * analysis take care of changing all of these to VariableBody (a named
 * reference to a local variable), NumericBody (a number constant) and
 * TreeNodeBody (a reference to an operator and its arguments). */
struct OperatorBody : public Printable {
    virtual ~OperatorBody() = default;
};

struct PairBody : public OperatorBody {
    PairBody() = default;
    PairBody(auto&& first, auto&& second) :
        first(AUX_FORWARD(first)),
        second(AUX_FORWARD(second))
    {}
    std::unique_ptr<OperatorBody> first;
    std::unique_ptr<OperatorBody> second;
    virtual std::ostream& print_to( std::ostream& ) const override;
};

struct SequenceBody : public OperatorBody {
    std::vector< std::unique_ptr<OperatorBody> > sequence;
    virtual std::ostream& print_to( std::ostream& ) const override;
};

struct TerminalBody : public OperatorBody {
    TerminalBody() = default;
    TerminalBody( auto&& t ) : name(AUX_FORWARD(t)) {}
    Token name;
    virtual std::ostream& print_to( std::ostream& ) const override;
};

/* These two structures are generated from TerminalBody
 * during semantic analysis. */
struct VariableBody : public OperatorBody {
    VariableBody() = default;
    VariableBody( auto&& t ) : name(AUX_FORWARD(t)) {}
    std::string name;
    virtual std::ostream& print_to( std::ostream& ) const override;
};

struct NumericBody : public OperatorBody {
    NumericBody() = default;
    NumericBody( auto&& t ) : value(AUX_FORWARD(t)) {}
    int value;
    virtual std::ostream& print_to( std::ostream& ) const override;
};

/* These structures are computed by the semantic analyzer.
 * Note that there is no need to differentiate between prefix
 * and postfix unary operators because all the semantic analysis
 * is already done when these structures are generated. */
struct TreeNodeBody : public OperatorBody {
    /* References to the operators are done via pointers.
     * Since each pointer points to a different object type,
     * it is better to mantain the pointer in each derived class
     * rendering this class empty. */
};

struct NullaryTreeBody : public TreeNodeBody {
    NullaryTreeBody() = default;
    NullaryTreeBody( auto&& op ) : op(AUX_FORWARD(op)) {}
    const NullaryOperator * op;
    virtual std::ostream& print_to( std::ostream& ) const override;
};
struct UnaryTreeBody : public TreeNodeBody {
    UnaryTreeBody() = default;
    UnaryTreeBody( auto&& op, auto&& variable ) :
        op(AUX_FORWARD(op)),
        variable(AUX_FORWARD(variable))
    {}
    std::unique_ptr<OperatorBody> variable;
    const UnaryOperator * op;
    virtual std::ostream& print_to( std::ostream& ) const override;
};
struct BinaryTreeBody : public TreeNodeBody {
    BinaryTreeBody() = default;
    BinaryTreeBody( auto&& op, auto&& l, auto&& r ) :
        op(AUX_FORWARD(op)),
        left(AUX_FORWARD(l)),
        right(AUX_FORWARD(r))
    {}
    const BinaryOperator * op;
    std::unique_ptr<OperatorBody> left, right;
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
    std::string format; // "F", "FX", "FY", "XFX", "YFX" etc.
    std::vector< std::unique_ptr<SignatureToken> > names;
    std::unique_ptr<OperatorBody> body;
    virtual std::ostream& print_to( std::ostream& ) const override;
};

#undef AUX_FORWARD

#endif // AST_H
