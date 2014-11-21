/* tree_build.cpp
 * Implementation of tree_build.h.
 */
#include "tree_build.h"

namespace {
    /* Builds the expression tree of the given operator body, changing every
     * SequenceBody and TerminalBody to suitable instances of VariableBody,
     * NumericBody and TreeNodeBody. */
    std::unique_ptr<OperatorBody> buildExpressionTree( const OperatorBody& ) {
        return nullptr; // FIXME
    }

} // anonymous namespace


std::string getMangledOperatorName( const OperatorDefinition& def ) {
    if( def.format == "f" )
        return dynamic_cast<const OperatorName&>(*def.names[0]).name.lexeme + ",F";
    if( def.format == "fx" || def.format == "fy" )
        return dynamic_cast<const OperatorName&>(*def.names[0]).name.lexeme + ",FX";
    if( def.format == "xf" || def.format == "yf" )
        return dynamic_cast<const OperatorName&>(*def.names[1]).name.lexeme + ",XF";
    // It can only be xfx, yfx, xfy.
    return dynamic_cast<const OperatorName&>(*def.names[1]).name.lexeme + ",XFX";
}


std::unique_ptr<NullaryOverload> buildNullaryTree( const OperatorDefinition& def ) {
    auto ptr = std::make_unique<NullaryOverload>();
    ptr->body = std::move( buildExpressionTree(*def.body) );
    return std::move( ptr );
}

std::unique_ptr<UnaryOverload> buildUnaryTree( const OperatorDefinition& def ) {
    auto ptr = std::make_unique<UnaryOverload>();
    if( def.format[0] == 'f' ) // we need def.names[1]->clone
        ptr->variable = std::make_unique<OperatorVariable>( *def.names[1] );
    else
        ptr->variable = std::make_unique<OperatorVariable>( *def.names[0] );

    ptr->body = std::move( buildExpressionTree(*def.body) );
    return std::move( ptr );
}

std::unique_ptr<BinaryOverload> buildBinaryTree( const OperatorDefinition& def ) {
    auto ptr = std::make_unique<BinaryOverload>();
    ptr->left = std::make_unique<OperatorVariable>( *def.names[0] );
    ptr->right = std::make_unique<OperatorVariable>( *def.names[2] );
    ptr->body = std::move( buildExpressionTree(*def.body) );
    return std::move( ptr );
}
