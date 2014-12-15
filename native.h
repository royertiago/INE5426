/* native.h
 * Hook that allows using C++ code in the operators of the language.
 */
#ifndef NATIVE_H
#define NATIVE_H

#include <ostream>
#include "ast.h"
#include "symbol_table.h"

struct NativeOperation : public OperatorBody {
    virtual std::unique_ptr<Variable> evaluate( const VariableTable& ) const = 0;
    virtual ~NativeOperation() = default;
    virtual NativeOperation * clone() const override = 0;
};

/* Template for binary numeric operations.
 * This class assumes either xfx, xfy or yfx format and variables
 * {X} and {Y}. */
template< typename Functor >
struct NativeBinaryNumericOperator : public NativeOperation {
    Functor f;
    std::string name;

    NativeBinaryNumericOperator( Functor f, std::string name ): f(f), name(name) {}

    virtual std::unique_ptr<Variable> evaluate( const VariableTable& table ) const override {
        auto X = table.retrieve("X");
        auto Y = table.retrieve("Y");
        return std::make_unique<Variable>( f(X->value, Y->value) );
    }
    virtual NativeBinaryNumericOperator * clone() const override {
        return new NativeBinaryNumericOperator{ f, name };
    }
    virtual std::ostream& print_to( std::ostream& os ) const override {
        return os << "{Native " << name << "}";
    }
};

template< typename Functor >
void insertNative( std::string name, std::string format, unsigned priority, Functor f ) {
    Token X, Y;
    X.lexeme = "X";
    Y.lexeme = "Y";
    auto ptr = std::make_unique<BinaryOverload>(
            name,
            std::make_unique<NativeBinaryNumericOperator<Functor>>(f, name),
            std::make_unique<RestrictedParameter>(X),
            std::make_unique<RestrictedParameter>(Y)
        );
    SymbolTable::insertOverload( name, format, priority, std::move(ptr) );
}

#endif // NATIVE_H
