/* semantic_analyser.cpp
 * Implementation of semantic_analyser.h
 */
#include "tree_build.h"
#include "operator.h"
#include "semantic_analyser.h"
#include "symbol_table.h"

SemanticAnalyser::SemanticAnalyser( std::unique_ptr<Parser>&& parser ) {
    parser_stack.emplace( std::move(parser) );
}

std::unique_ptr<Statement> SemanticAnalyser::next() {
    if( !_next )
        compute_next();
    return std::move(_next);
}

const Statement * SemanticAnalyser::peek() {
    if( !_next )
        compute_next();
    return _next.get();
}

bool SemanticAnalyser::has_next() const {
    return !parser_stack.empty() && parser_stack.top()->has_next();
}

void SemanticAnalyser::compute_next() {
    try {
        auto ptr = parser_stack.top()->next();

        if( !parser_stack.top()->has_next() )
            parser_stack.pop();

        if( typeid(*ptr) != typeid(OperatorDefinition) ) {
            if( auto include = dynamic_cast<IncludeCommand *>(ptr.get()) ) {
                parser_stack.emplace(
                        std::make_unique<Parser>(include->filename.lexeme.c_str())
                    );
            }
            if( auto category = dynamic_cast<CategoryDefinition *>(ptr.get()) )
                SymbolTable::insertCategory(category->name.lexeme );
            _next = std::move( ptr );
            return;
        }
        OperatorDefinition & def = static_cast<OperatorDefinition&>(*ptr);
        if( def.format == "f" )
            _next = std::move( buildNullaryTree(def) );
        else if( def.format == "fx"
              || def.format == "fy"
              || def.format == "xf"
              || def.format == "yf" )
            _next = std::move( buildUnaryTree(def) );
        else
            _next = std::move( buildBinaryTree(def) );

        OperatorOverload & op = static_cast<OperatorOverload&>(*_next);
        SymbolTable::insertOverload( op.name, def.format, def.priority,
                std::move(std::unique_ptr<OperatorOverload>(op.clone())) );
    }
    catch ( parse_error & err ) {
        parser_stack.top()->panic();
        throw;
    }
}

// GAMBIARRRRA
std::pair< std::unique_ptr<SemanticAnalyser>, std::unique_ptr<OperatorBody> >
    parse_single_line( std::string line )
{
    Lexer lex(( std::string(line) )); // move a copy
    // Most vexing parse rule...
    if( Token::declarator(lex.peek()) )
        return std::make_pair(
                std::make_unique<SemanticAnalyser>( std::make_unique<Parser>(std::move(line)) ),
                std::unique_ptr<OperatorBody>(nullptr)
        );

    Parser parser( "f 0 dummy " + line );
    auto ptr = parser.next();

    auto * op = dynamic_cast<OperatorDefinition *>( ptr.get() );
    if( !op )
        throw semantic_error( "No valid parsing found" );

    return std::make_pair(
                std::unique_ptr<SemanticAnalyser>(nullptr),
                std::move(buildNullaryTree(*op)->body)
        );
}
