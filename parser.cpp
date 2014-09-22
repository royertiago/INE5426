/* parser.cpp
 * Implementation of parser.h
 */
#include "parser.h"
#include "exceptions.h"

namespace {
    /* Parse an include directive.
     * The first token will be assumed have Token::INCLUDE as id. */
    std::unique_ptr<IncludeCommand> parse_include( Lexer& );

    /* Parse a category definition.
     * The first token will be assumed have Token::CATEGORY as id. */
    std::unique_ptr<CategoryDefinition> parse_category( Lexer& );

    /* Parse an operator definition.
     * The first token will be assumed have Token::F, Token::FX etc. as id. */
    std::unique_ptr<OperatorDefinition> parse_operator( Lexer& ) {
        return nullptr;
    }
}

void Parser::compute_next() {
    if( !_alex.has_next() ) {
        _next = nullptr;
        return;
    }
    switch( _alex.peek().id ) {
        case Token::INCLUDE:
                _next = parse_include( _alex );
                return;
        case Token::CATEGORY:
                _next = parse_category( _alex );
                return;
        case Token::F:
        case Token::FX:
        case Token::FY:
        case Token::XF:
        case Token::YF:
        case Token::XFX:
        case Token::YFX:
        case Token::XFY:
                _next = parse_operator( _alex );
                return;
        default:
                throw parse_error( "Expected include, category or operator", _alex.next() );
    }
}

namespace {
std::unique_ptr<IncludeCommand> parse_include( Lexer& alex ) {
    alex.next();
    if( alex.peek().id != Token::IDENTIFIER )
        throw parse_error( "Expected identifier (parse_include)", alex.next() );

    auto ptr = std::make_unique<IncludeCommand>();
    ptr->filename = alex.next();
    return std::move( ptr );
}

std::unique_ptr<CategoryDefinition> parse_category( Lexer& alex ) {
    alex.next();
    if( alex.peek().id != Token::IDENTIFIER )
        throw parse_error( "Expected identifier (parse_category)", alex.next() );

    auto ptr = std::make_unique<CategoryDefinition>();
    ptr->name = alex.next();
    return std::move( ptr );
}
} // anonymous namespace
