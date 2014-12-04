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
        const LocalSymbolTable& table )
{
    struct Data {
        std::unique_ptr< OperatorBody > data = nullptr;
        bool valid = false;
        unsigned priority = 0;
    };
    std::vector<std::vector<Data>> dp(
            body.sequence.size(),
            std::vector<Data>( body.sequence.size() )
        );
    /* dp[i][j] represents the parse tree for the subsequence
     * consisting of the 'tokens' body.sequence[i, i+1, ..., j].
     *
     * We will ignore ambiguous constructions and call them as 'invalid'.
     * Although it is possible to parse around this limitations, the algorithm
     * is even more complex than the one presented here. */

    for( int i = 0; i < body.sequence.size(); ++i )
        try {
            const TerminalBody & tbody = dynamic_cast<TerminalBody&>(*body.sequence[i]);
            if( GlobalSymbolTable::instance.existsNullaryOperator(tbody.token.lexeme) ) {
                dp[i][i].date = std::make_unique<NullaryTreeBody>(
                        GlobalSymbolTable::instance.retrieveNullaryOperator(tbody.token.lexeme)
                    );
                dp[i][i].valid = true;
                dp[i][i].priority = GlobalSymbolTable::instance.operatorPriority(
                        tbody.token.lexeme
                    );
            }
        } catch( std::bad_cast & ex ) {
            try{
                dp[i][i].data = std::move( buildExpressionTree(*body.sequence[i], table) );
                dp[i][i].valid = true;
            } catch( semantic_error & ex ) {
                /* The only lines that throw exceptions we handle are the
                 * buildExpressionTree (that does not take into account the possibility
                 * of having a nullary operator) and the dynamic_cast (that we do
                 * in order to test exactly this possibility).
                 *
                 * The occurrence of both errors means that the tree below body.sequence[i]
                 * cannot be parsed properly as a single atom, so we are correct
                 * to keep the default invalid state. */
            }
        }

    for( int d = 0; d < body.sequence.size(); ++d )
        for( int i = 0, j = d + i; j < body.sequence.size(); ++i, ++j ) {
            /* First, let's try to interpret sequence[i, i+1,...,j] as a prefix
             * operator followed by its operands. */
            if( dp[i+1][j].valid &&
                TerminalBody * tbody = dynamic_cast<const TerminalBody*>(body.sequence[i].get()))
            {
                std::string name = tbody->token.lexeme;
                if( dp[i+1][j].priority < GlobalSymbolTable::instance.maximumPrefixPriority(name) ) {
                    dp[i][j].data = std::make_unique<UnaryTreeBody>(
                            GlobalSymbolTable::instance.retrievePrefixOperator(name),
                            dp[i+1][j].data->clone()
                        );
                    dp[i][j].priority = GlobalSymbolTable::instance.operatorPriority(name);
                    dp[i][j].state = Data::VALID;
                }
            }
            /* Now, we will try an interpretation as postfix operator. */
            if( dp[i][j-1].valid &&
                TerminalBody * tbody = dynamic_cast<const TerminalBody*>(body.sequence[j].get()))
            {
                std::string name = tbody->token.lexeme;
                if( dp[i][j-1].priority < GlobalSymbolTable::instance.maximumPrefixPriority(name) ) {
                    if( dp[i][j].valid ) {
                        dp[i][j].valid = false;
                        continue;
                    }
                    dp[i][j].data = std::make_unique<UnaryTreeBody>(
                            GlobalSymbolTable::instance.retrievePrefixOperator(name),
                            dp[i][j-1].data->clone()
                        );
                    dp[i][j].priority = GlobalSymbolTable::instance.operatorPriority(name);
                    dp[i][j].state = Data::VALID;
                }
            }
            /* Finnaly, binary overloads.
             * We are updating dp[i][j] and will try to form a new parse
             * tree whose root is the operator at body.sequence[k]. */
            for( int k = i+1; k <= j-1; ++k )
                if( dp[i][k-1].valid && dp[k+1][j].valid &&
                    TerminalBody * tbody = dynamic_cast<const TerminalBody*>(body.sequence[j].get()))
                {
                    std::string name = tbody->name;
                    if( dp[i][k-1].priority < GlobalSymbolTable::instance.maximumLeftPriority(name)
                     && dp[k+1][j].priority < GlobalSymbolTable::instance.maximumRightPriority(name))
                    {
                        if( dp[i][j].valid ) {
                            dp[i][j].valid = false;
                            goto end_external_loop;
                        }
                        dp[i][j].data = std::make_unique<BinaryOperator>(
                            GlobalSymbolTable::instance.retrieveBinaryOperator(name),
                            dp[i][k-1].data->clone();
                            dp[k+1][j].data->clone();
                        );
                        dp[i][j].valid = true;
                        dp[i][j].priority = GlobalSymbolTable::instance.operatorPriority(name);
                    }
                }

end_external_loop:;
        }

    if( !pd[0][body.sequence.size()-1].valid )
        throw semantic_error( "No viable semantic parsing found for given operators." );

    return std::move( pd[0][body.sequence.size()-1].data );
}

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

} // anonymous namespace
