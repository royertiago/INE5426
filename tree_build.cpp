/* tree_build.cpp
 * Implementation of tree_build.h.
 */
#include <cstdlib>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include "exceptions.h"
#include "tree_build.h"
#include "symbol_table.h"

namespace {
    /* Builds the expression tree of the given operator body, changing every
     * SequenceBody and TerminalBody to suitable instances of VariableBody,
     * NumericBody and TreeNodeBody. */
    std::unique_ptr<OperatorBody> buildExpressionTree( const OperatorBody&, const LocalSymbolTable& );

    /* Aggregates all the variables' names used inside the passed OperatorParameter. */
    LocalSymbolTable collectVariables( const OperatorParameter & );

} // anonymous namespace

std::unique_ptr<NullaryOverload> buildNullaryTree( const OperatorDefinition& def ) {
    auto ptr = std::make_unique<NullaryOverload>();
    ptr->body = std::move( buildExpressionTree(*def.body, LocalSymbolTable()) );
    ptr->name = static_cast<const OperatorName&>(*def.names[0]).name.lexeme;
    return std::move( ptr );
}

std::unique_ptr<UnaryOverload> buildUnaryTree( const OperatorDefinition& def ) {
    auto ptr = std::make_unique<UnaryOverload>();
    LocalSymbolTable table;
    if( def.format[0] == 'f' ) {
        ptr->variable.reset(static_cast<const OperatorParameter&>(*def.names[1]).clone());
        table = collectVariables( static_cast<const OperatorParameter&>(*def.names[1]) );
        ptr->name = static_cast<const OperatorName&>(*def.names[0]).name.lexeme;
    }
    else {
        ptr->variable.reset(static_cast<const OperatorParameter&>(*def.names[0]).clone());
        table = collectVariables( static_cast<const OperatorParameter&>(*def.names[0]) );
        ptr->name = static_cast<const OperatorName&>(*def.names[1]).name.lexeme;
    }

    ptr->body = std::move( buildExpressionTree(*def.body, table) );
    return std::move( ptr );
}

std::unique_ptr<BinaryOverload> buildBinaryTree( const OperatorDefinition& def ) {
    auto ptr = std::make_unique<BinaryOverload>();
    ptr->left.reset(static_cast<OperatorParameter&>( *def.names[0] ).clone());
    ptr->name = static_cast<OperatorName&>(*def.names[1]).name.lexeme;
    ptr->right.reset(static_cast<OperatorParameter&>( *def.names[2] ).clone());
    auto table = collectVariables( *ptr->left ).merge(collectVariables( *ptr->right ));
    ptr->body = std::move( buildExpressionTree(*def.body, table) );
    return std::move( ptr );
}

namespace {
typedef std::unique_ptr<OperatorBody> (*ExpressionTreeBuilder)(
        const OperatorBody&,
        const LocalSymbolTable&
    );

std::unique_ptr<OperatorBody> buildExpressionPairBody(
        const PairBody& body,
        const LocalSymbolTable& table )
{
    return std::make_unique<PairBody>(
            std::move( buildExpressionTree( *body.first,  table ) ),
            std::move( buildExpressionTree( *body.second, table) )
            );
}

std::unique_ptr<OperatorBody> buildExpressionSequenceBody(
        const SequenceBody& body,
        const LocalSymbolTable& table )
{
    struct Data {
        std::unique_ptr< OperatorBody > data = nullptr;
        bool valid = false;
        unsigned priority = -1;
    };
    std::vector<std::vector<Data>> dp;
    for( unsigned i = 0; i < body.sequence.size(); i++ ) {
        dp.emplace_back( std::vector<Data>( body.sequence.size() ) );
    }
    /* dp[i][j] represents the parse tree for the subsequence
     * consisting of the 'tokens' body.sequence[i, i+1, ..., j].
     *
     * We will ignore ambiguous constructions and call them as 'invalid'.
     * Although it is possible to parse around this limitations, the algorithm
     * is even more complex than the one presented here. */

    for( unsigned i = 0; i < body.sequence.size(); ++i )
        try {
            dp[i][i].data = std::move( buildExpressionTree(*body.sequence[i], table) );
            dp[i][i].valid = true;
            if( auto op = dynamic_cast<const NullaryTreeBody *>( dp[i][i].data.get() ) )
                dp[i][i].priority = GlobalSymbolTable::nullaryOperatorPriority(op->op->name);
            else
                dp[i][i].priority = 0;
        } catch( semantic_error & ) {
            /* The occurrence of an exception means that the tree below body.sequence[i]
             * cannot be parsed properly as a single atom, so we are correct
             * to keep the default invalid state. */
        }

    for( unsigned d = 1; d < body.sequence.size(); ++d )
        for( unsigned i = 0, j = d + i; j < body.sequence.size(); ++i, ++j ) {
            /* First, let's try to interpret sequence[i, i+1,...,j] as a prefix
             * operator followed by its operands. */
            if( const TerminalBody * tbody = !dp[i+1][j].valid ? nullptr :
                                        dynamic_cast<const TerminalBody*>(body.sequence[i].get()))
            {
                std::string name = tbody->name.lexeme;
                if( GlobalSymbolTable::existsPrefixOperator(name) &&
                    dp[i+1][j].priority <= GlobalSymbolTable::maximumPrefixPriority(name) )
                {
                    dp[i][j].data = std::move( std::make_unique<UnaryTreeBody>(
                            GlobalSymbolTable::retrievePrefixOperator(name),
                            dp[i+1][j].data->clone()
                        ) );
                    dp[i][j].priority = GlobalSymbolTable::prefixOperatorPriority(name);
                    dp[i][j].valid = true;
                }
            }
            /* Now, we will try an interpretation as postfix operator. */
            if( const TerminalBody * tbody = !dp[i][j-1].valid ? nullptr :
                                        dynamic_cast<const TerminalBody*>(body.sequence[j].get()))
            {
                std::string name = tbody->name.lexeme;
                if( GlobalSymbolTable::existsPostfixOperator(name) &&
                    dp[i][j-1].priority <= GlobalSymbolTable::maximumPostfixPriority(name) )
                {
                    if( dp[i][j].valid ) {
                        dp[i][j].valid = false;
                        continue;
                    }
                    dp[i][j].data = std::move( std::make_unique<UnaryTreeBody>(
                            GlobalSymbolTable::retrievePostfixOperator(name),
                            dp[i][j-1].data->clone()
                        ) );
                    dp[i][j].priority = GlobalSymbolTable::postfixOperatorPriority(name);
                    dp[i][j].valid = true;
                }
            }
            /* Finnaly, binary overloads.
             * We are updating dp[i][j] and will try to form a new parse
             * tree whose root is the operator at body.sequence[k]. */
            for( unsigned k = i+1; k <= j-1; ++k )
                if( const TerminalBody * tbody = !dp[k+1][j].valid ? nullptr :
                                            dynamic_cast<const TerminalBody*>(body.sequence[k].get()))
                {
                    std::string name = tbody->name.lexeme;
                    if( GlobalSymbolTable::existsBinaryOperator(name)
                     && dp[i][k-1].priority <= GlobalSymbolTable::maximumLeftPriority(name)
                     && dp[k+1][j].priority <= GlobalSymbolTable::maximumRightPriority(name))
                    {
                        if( dp[i][j].valid ) {
                            dp[i][j].valid = false;
                            goto end_external_loop;
                        }
                        dp[i][j].data = std::move( std::make_unique<BinaryTreeBody>(
                            GlobalSymbolTable::retrieveBinaryOperator(name),
                            dp[i][k-1].data->clone(),
                            dp[k+1][j].data->clone()
                            ) );
                        dp[i][j].valid = true;
                        dp[i][j].priority = GlobalSymbolTable::binaryOperatorPriority(name);
                    }
                }

end_external_loop:;
        }

    if( !dp[0][body.sequence.size()-1].valid )
        throw semantic_error( "No viable semantic parsing found for given operators." );

    return std::move( dp[0][body.sequence.size()-1].data );
}

std::unique_ptr<OperatorBody> buildExpressionTerminalBody(
        const TerminalBody& body,
        const LocalSymbolTable& table )
{
    if( body.name.id == Token::NUM )
        return std::make_unique<NumericBody>(
                std::strtol( body.name.lexeme.c_str(), 0, 10 ) // porque foda-se o Melga
        );
    if( table.contains( body.name.lexeme ) )
        return std::make_unique<VariableBody>( body.name.lexeme );

    if( GlobalSymbolTable::existsNullaryOperator(body.name.lexeme) )
        return std::make_unique<NullaryTreeBody>(
                GlobalSymbolTable::retrieveNullaryOperator(body.name.lexeme)
            );

    throw semantic_error( "Terminal " + body.name.lexeme + "is not a number,"
            " a variable or a unary operator." );
}


#define AUX_TYPE(type)                                                                              \
    {                                                                                               \
        std::type_index(typeid(type)),                                                              \
        static_cast<ExpressionTreeBuilder>(                                                         \
            []( const OperatorBody& body, const LocalSymbolTable& table ) {                         \
                return std::move(buildExpression##type( dynamic_cast<const type&>(body), table));   \
            }                                                                                       \
        )                                                                                           \
    }

std::unique_ptr<OperatorBody> buildExpressionTree(
        const OperatorBody& body,
        const LocalSymbolTable& table )
{
    std::unordered_map<std::type_index, ExpressionTreeBuilder> functions = {
        AUX_TYPE(PairBody),
        AUX_TYPE(SequenceBody),
        AUX_TYPE(TerminalBody),
    };
    return std::move( functions.at(std::type_index(typeid(body)))(body, table) );
}

typedef void(* InsertorFunction )( const OperatorParameter &, LocalSymbolTable & );

void insertVariables( const OperatorParameter & var, LocalSymbolTable & table ) {
    // A 'switch-case' with types
    static std::unordered_map<std::type_index, InsertorFunction > jump_table =
    {
        {
            std::type_index(typeid(NamedParameter)),
            static_cast<InsertorFunction>(
                []( const OperatorParameter & var, LocalSymbolTable & table ) {
                    auto & nvar = dynamic_cast<const NamedParameter&>(var);
                    table.insert( nvar.name.lexeme );
                })
        },
        {
            std::type_index(typeid(RestrictedParameter)),
            static_cast<InsertorFunction>(
                []( const OperatorParameter & var, LocalSymbolTable & table ) {
                    auto & nvar = dynamic_cast<const RestrictedParameter&>(var);
                    table.insert( nvar.name.lexeme );
                })
        },
        {
            std::type_index(typeid(NumericParameter)),
            static_cast<InsertorFunction>(
                []( const OperatorParameter &, LocalSymbolTable & ) {
                    // We do not need to save numbers in the symbol table.
                })
        },
        {
            std::type_index(typeid(PairParameter)),
            static_cast<InsertorFunction>(
                []( const OperatorParameter & var, LocalSymbolTable & table ) {
                    auto & nvar = dynamic_cast<const PairParameter&>(var);
                    insertVariables( *nvar.first, table );
                    insertVariables( *nvar.second, table );
                })
        },
    };
    jump_table.at(typeid(var))( var, table );
}

LocalSymbolTable collectVariables( const OperatorParameter & var ) {
    LocalSymbolTable table;
    insertVariables( var, table );
    return table;
}

} // anonymous namespace
