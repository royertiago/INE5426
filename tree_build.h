/* tree_build.h
 * Set of mutually recursive functions that recieves an OperatorDefinition
 * directly from the syntatical analyser, and outputs a corresponding
 * OperatorOverload, satisfying the class invariant estabilished in operator.h.
 *
 * Here is only shown the interface to the set; the functions takes an
 * OperatorDefinition and returns either a NullaryOverload, an UnaryOverload
 * or a BinaryOverload.
 */
#ifndef TREE_BUILD_H
#define TREE_BUILD_H

#include "ast.h"
#include "operator.h"

// No checking is done to assure that the function receives the correct object.
std::unique_ptr<NullaryOverload> buildNullaryTree( const OperatorDefinition& );
std::unique_ptr<UnaryOverload>   buildUnaryTree  ( const OperatorDefinition& );
std::unique_ptr<BinaryOverload>  buildBinaryTree ( const OperatorDefinition& );

/* Since an OperatorOverload does not holds the operator name, we need a
 * function to extract it separately.
 *
 * This function does the mangling corretly, according to mangling.md. */
std::string getMangledOperatorName( const OperatorDefinition& );

#endif // TREE_BUILD_H
