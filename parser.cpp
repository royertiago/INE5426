/* parser.cpp
 * Implementation of parser.h
 */
#include <cstdlib>
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
    std::unique_ptr<OperatorDefinition> parse_operator( Lexer& );

    /* Parse a variable definition, in operator headers. */
    std::unique_ptr<OperatorVariable> parse_variable( Lexer& );

    /* Parse an operator body. */
    std::unique_ptr<OperatorBody> parse_body( Lexer& );
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
        throw parse_error( "Expected identifier after 'include' token", alex.next() );

    auto ptr = std::make_unique<IncludeCommand>();
    ptr->filename = alex.next();
    return std::move( ptr );
}

std::unique_ptr<CategoryDefinition> parse_category( Lexer& alex ) {
    alex.next();
    if( alex.peek().id != Token::IDENTIFIER )
        throw parse_error( "Expected identifier after 'category' token", alex.next() );

    auto ptr = std::make_unique<CategoryDefinition>();
    ptr->name = alex.next();
    return std::move( ptr );
}

std::unique_ptr<OperatorDefinition> parse_operator( Lexer& alex ) {
    auto ptr = std::make_unique<OperatorDefinition>();
    ptr->format = alex.next().lexeme;

    if( alex.peek().id != Token::NUM ) throw parse_error( "Expected priority", alex.next() );
    ptr->priority = std::atoi(alex.next().lexeme.c_str());


    for( char c : ptr->format )
        if( c == 'f' )
            ptr->names.emplace_back( std::make_unique<OperatorName>(alex.next()) );
        else
            ptr->names.emplace_back( std::move(parse_variable( alex )) );

    ptr->body = parse_body( alex );
    if( alex.peek().id == '}' )
        throw parse_error( "Right brace never opened", alex.next() );
    if( alex.has_next() && !Token::declarator(alex.peek()) )
        throw parse_error( "Unfinished operator body", alex.next() );
    return std::move( ptr );
}

std::unique_ptr<OperatorVariable> parse_variable( Lexer& ) {
    return nullptr; // FIXME
}

std::unique_ptr<OperatorBody> parse_body( Lexer& alex ) {
    auto ptr = std::make_unique<SequenceBody>();
    while( true ) {
        if( Token::sequence(alex.peek()) )
            ptr->sequence.emplace_back( std::make_unique<TerminalBody>(alex.next()) );
        else if( alex.peek().id == '{' ) {
            Token tok = alex.next();
            ptr->sequence.emplace_back( std::move(parse_body(alex)) );
            if( alex.peek().id != '}' )
                throw parse_error( "Left brace never closed", tok );
            else
                alex.next();
        }
        else
            break;
    }
    if( ptr->sequence.empty() )
        throw parse_error( "Invalid empty token sequence", alex.peek() );
    if( alex.peek().id == ',' ) {
        alex.next();
        return std::make_unique<PairBody>( std::move(ptr), parse_body(alex) );
    }
    return std::move(ptr);
}

} // anonymous namespace
