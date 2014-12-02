/* tree_build.cpp
 * Implementation of tree_build.h.
 */
#include <cstdlib>
#include <typeinfo>
#include <typeindex>
#include "tree_build.h"

namespace {
    /* Builds the expression tree of the given operator body, changing every
     * SequenceBody and TerminalBody to suitable instances of VariableBody,
     * NumericBody and TreeNodeBody. */
    std::unique_ptr<OperatorBody> buildExpressionTree( const OperatorBody&, const LocalSymbolTable& );

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

namespace {
typedef std::unique_ptr<OperatorBody> (*ExpressionTreeBuilder)(
        const OperatorBody&*,
        const LocalSymbolTable&
    );

std::unique_ptr<OperatorBody> buildExpressionPairBody(
        const PairBody& body,
        const LocalSymbolTable& table )
{
    return std::make_unique<PairBody>(
            std::move( buildExpressionTree(body.first) ),
            std::move( buildExpressionTree(body.second) )
            );
}
std::unique_ptr<OperatorBody> buildExpressionSequenceBody(
        const SequenceBody& body,
        const LocalSymbolTable& table );
std::unique_ptr<OperatorBody> buildExpressionTerminalBody(
        const TerminalBody& body,
        const LocalSymbolTable& table )
{
    if( body.name.id == NUM )
        return std::make_unique<NumericBody>(
                std::strtol( body.name.lexeme.begin(), 0, 10 ) // porque foda-se o Melga
        );
    if( table.contains( body.name.lexeme ) )
        return std::make_unique<VariableBody>( body.name.lexeme );

    throw semantic_error( "Terminal is not a number neiter a variable", body.name );
    // TODO: maybe this behavior is unwanted according to buildExpressionSequenceBody.
}


#define AUX_TYPE(type)                                                                      \
    {                                                                                       \
        type_index(typeid(type)),                                                           \
        static_cast<ExpressionTreeBuilder>(                                                 \
            []( const OperatorBody& body, const LocalSymbolTable& table ) {                 \
                return std::move(buildExpression##type( dynamic_cast<type&>(body), table)); \
            }                                                                               \
        )                                                                                   \
    }

std::unique_ptr<OperatorBody> buildExpressionTree(
        const OperatorBody& body,
        const LocalSymbolTable& table )
{
    std::map<type_index, ExpressionTreeBuilder> functions = {
        AUX_TYPE(PairBody),
        AUX_TYPE(SequenceBody),
        AUX_TYPE(TerminalBody),
    };
    return std::move( functions.at(type_index(typeid(body)))(body, table) );
}

}
