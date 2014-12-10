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
 */
struct OperatorOverload : public Statement {
    std::unique_ptr<OperatorBody> body;
    /* Class invariant: the only instances in the tree below
     * 'body' are TreeNodeBody, NumericBody, VariableBody
     * and PairBody. In particular, TerminalBody and SequenceBody
     * shall have had gone through semantic analysis and replaced
     * with suitable instances of VariableBody and NumericBody,
     * and of TreeNodeBody, respectively. */
    virtual std::ostream& print_to( std::ostream& ) const override;
    virtual OperatorOverload * clone() const override;
};

struct NullaryOverload : public OperatorOverload {
    // There is no signature.
    virtual std::ostream& print_to( std::ostream& ) const override;
    virtual NullaryOverload * clone() const override;
};

struct UnaryOverload : public OperatorOverload {
    std::unique_ptr<OperatorVariable> variable;
    virtual std::ostream& print_to( std::ostream& ) const override;
    virtual UnaryOverload * clone() const override;
};
struct BinaryOverload : public OperatorOverload {
    std::unique_ptr<OperatorVariable> left;
    std::unique_ptr<OperatorVariable> right;
    virtual std::ostream& print_to( std::ostream& ) const override;
    virtual BinaryOverload * clone() const override;
};

template <typename Overload>
struct OperatorBase : public Symbol {
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
};
struct UnaryOperator : public OperatorBase<NullaryOverload> {
    unsigned operand_priority;
};
struct BinaryOperator : public OperatorBase<NullaryOverload> {
    unsigned left_priority;
    unsigned right_priority;
};

#endif // OPERATOR_H
