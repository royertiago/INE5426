/* semantic_analyser.cpp
 * Implementation of semantic_analyser.h
 */
#include "tree_build.h"
#include "operator.h"
#include "semantic_analyser.h"
#include "symbol_table.h"

void SemanticAnalyser::set_parser( std::unique_ptr<Parser>&& parser ) {
    _current_parser = std::move(parser);
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
    return _current_parser && _current_parser->has_next();
}

void SemanticAnalyser::compute_next() {
    try {
        auto ptr = _current_parser->next();

        if( typeid(*ptr) != typeid(OperatorDefinition) ) {
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
        _current_parser->panic();
        throw;
    }
}
