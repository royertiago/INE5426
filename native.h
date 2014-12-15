/* native.h
 * Hook that allows using C++ code in the operators of the language.
 */
#ifndef NATIVE_H
#define NATIVE_H

#include "ast.h"

struct NativeOperation : public OperatorBody {
    virtual std::unique_ptr<Variable> evaluate( const VariableTable& ) = 0;
    virtual ~NativeOperation() = default;
    virtual OperatorBody * clone() const override = 0;
};

#endif // NATIVE_H
