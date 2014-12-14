/* operator.h
 * Structure that holds the information about operators in the program.
 *
 * There are four operator types (nullary, prefix, postfix and binary). Each
 * operator can have several possible overloads. Each overload have a variable
 * tree (that is used to resolve overload at runtime) and a corresponding
 * body - an tree structure, produced by the "semantic-syntatic" analyzer.
 */
#ifndef OPERATOR_H
#define OPERATOR_H

#include "printable.h"
#include "ast.h"
#include "symbol.h"

#define AUX_FORWARD(var) std::forward<decltype(var)>(var)

/* OperatorOverload contains all the information about a specific
 * overload of an operator. The overloads are selected at runtime.
 *
 * OperatorOverload is a common base that have the body of the
 * operator. The following three classes extend it to include
 * information about the signature.
 *
 * Example: consider the following operator definition:
 * xf 800 {7, X, {Y}} alce
 *      7 + Y, X
 *
 * The body of this overload ('7 + Y, X') is stored in one instance
 * of this class, while the signature ('{7, X, {Y}}') is stored in
 * the derived class UnaryOverload.
 *
 * Note that there is no need of differentiating beetween prefix
 * and postfix: each node has a pointer to its version of the operator.
 *
 * The evaluation of each overload is done by calling a method
 * named 'compute' in each class, that have a signature according
 * to its type.
 * The method 'compute' first decompose the variable using the 'decompose'
 * method from the classes below OperatorParameter. These methods might
 * fail; in this case, a semantic_error is thrown.
 */
struct OperatorOverload : public Statement {
    OperatorOverload() = default;
    OperatorOverload( auto&& n, auto&& b ) :
        name( AUX_FORWARD(n) ),
        body( AUX_FORWARD(b) )
    {}
    std::string name;
    std::unique_ptr<OperatorBody> body;
    /* Class invariant: the only instances in the tree below
     * 'body' are TreeNodeBody, NumericBody, VariableBody
     * and PairBody. In particular, TerminalBody and SequenceBody
     * shall have had gone through semantic analysis and replaced
     * with suitable instances of VariableBody and NumericBody,
     * and of TreeNodeBody, respectively. */
    virtual std::ostream& print_to( std::ostream& ) const = 0;
    virtual OperatorOverload * clone() const = 0;
};

struct NullaryOverload : public OperatorOverload {
    NullaryOverload() = default;
    NullaryOverload( auto&& n, auto&& b ) :
        OperatorOverload( AUX_FORWARD(n), AUX_FORWARD(b) )
    {}
    std::unique_ptr<Variable> compute() const;
    virtual std::ostream& print_to( std::ostream& ) const override;
    virtual NullaryOverload * clone() const override;
};

struct UnaryOverload : public OperatorOverload {
    UnaryOverload() = default;
    UnaryOverload( auto&& n, auto&& b, auto&& v ) :
        OperatorOverload( AUX_FORWARD(n), AUX_FORWARD(b) ),
        variable( AUX_FORWARD(v) )
    {}
    std::unique_ptr<OperatorParameter> variable;
    std::unique_ptr<Variable> compute( std::unique_ptr<Variable>&& ) const;
    virtual std::ostream& print_to( std::ostream& ) const override;
    virtual UnaryOverload * clone() const override;
};
struct BinaryOverload : public OperatorOverload {
    BinaryOverload() = default;
    BinaryOverload( auto&& n, auto&& b, auto&& l, auto&& r ) :
        OperatorOverload( AUX_FORWARD(n), AUX_FORWARD(b) ),
        left( AUX_FORWARD(l) ),
        right( AUX_FORWARD(r) )
    {}
    std::unique_ptr<OperatorParameter> left;
    std::unique_ptr<OperatorParameter> right;
    std::unique_ptr<Variable> compute(
            std::unique_ptr<Variable>&& left,
            std::unique_ptr<Variable>&& right ) const;
    virtual std::ostream& print_to( std::ostream& ) const override;
    virtual BinaryOverload * clone() const override;
};

/* Finally, we can construct Operators as sets of Overloads.
 *
 * Each operator have a 'compute' method, analog to its
 * Overload counterpart. Here, the operator attempts each
 * overload in insertion order until some operator correctly
 * returns, or raises an exception (a semantic_error) if
 * no valid overload is found. */
template <typename Overload>
struct OperatorBase : public Symbol {
    OperatorBase( std::string name ) : Symbol( name ) {}
    std::vector<std::unique_ptr<Overload>> overloads;
    void insert( std::unique_ptr<OperatorOverload>&& overload ) {
        Overload * ptr = &dynamic_cast<Overload&>( *overload );
        overload.release();
        /* We needed to do this in two steps in order to not leak
         * memory if the cast throws an exception. */
        overloads.emplace_back( ptr );
    }
    unsigned priority;
};

struct NullaryOperator : public OperatorBase<NullaryOverload> {
    std::unique_ptr<Variable> compute();
    NullaryOperator( std::string name ) : OperatorBase<NullaryOverload>( name ) {}
    std::unique_ptr<Variable> compute() const;
};
struct UnaryOperator : public OperatorBase<UnaryOverload> {
    UnaryOperator( std::string name ) : OperatorBase<UnaryOverload>( name ) {}
    unsigned operand_priority;
    std::unique_ptr<Variable> compute( std::unique_ptr<Variable>&& ) const;
};
struct BinaryOperator : public OperatorBase<BinaryOverload> {
    BinaryOperator( std::string name ) : OperatorBase<BinaryOverload>( name ) {}
    unsigned left_priority;
    unsigned right_priority;
    std::unique_ptr<Variable> compute(
            std::unique_ptr<Variable>&& left,
            std::unique_ptr<Variable>&& right ) const;
};

#endif // OPERATOR_H
