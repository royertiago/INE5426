/* semantic_analyser.cpp
 * Implementation of semantic_analyser.h
 */
#include "semantic_analyser.h"

void SemanticAnalyser::set_parser( std::unique_ptr<Paser>&& parser ) {
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

bool Parser::has_next() const {
    return _current_parser && _current_parser.has_next();
}

void SemanticAnalyser::compute_next() {
    try {
        auto ptr = _current_parser->next();

        if( typeid(ptr.get()) != typeid(OperatorDefinition*) ) {
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

        GlobalSymbolTable::insertOverload( _next->name, def.format, def.priority, _next.clone() );
    }
    catch ( lexical_error & err ) {
        _current_parser->panic();
        throw;
    }
}
