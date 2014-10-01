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

    /* Parse the sequence "comma-variable-comma-variable-comma-variable"...
     * in a variable definition that must be structured in pairs.
     * The parsing starts after a comma. */
    std::unique_ptr<OperatorVariable> parse_variable_list( Lexer& );

    /* Parse an operator body. */
    std::unique_ptr<OperatorBody> parse_body( Lexer& );

    /* Converts an string to a tuple. */
    template< typename Pair >
    std::unique_ptr<Pair> string_to( Token tok ) {
        throw parse_error( "Unimplemented feature (string to tuple conversion).", tok );
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
                throw parse_error( "Expected include, category or operator", _alex.peek() );
    }
}

namespace {

std::unique_ptr<IncludeCommand> parse_include( Lexer& alex ) {
    alex.next();
    if( alex.peek().id != Token::IDENTIFIER )
        throw parse_error( "Expected identifier after 'include' token", alex.peek() );

    auto ptr = std::make_unique<IncludeCommand>();
    ptr->filename = alex.next();
    return std::move( ptr );
}

std::unique_ptr<CategoryDefinition> parse_category( Lexer& alex ) {
    alex.next();
    if( alex.peek().id != Token::IDENTIFIER )
        throw parse_error( "Expected identifier after 'category' token", alex.peek() );

    auto ptr = std::make_unique<CategoryDefinition>();
    ptr->name = alex.next();
    return std::move( ptr );
}

std::unique_ptr<OperatorDefinition> parse_operator( Lexer& alex ) {
    auto ptr = std::make_unique<OperatorDefinition>();
    ptr->format = alex.next().lexeme;

    if( alex.peek().id != Token::NUM ) throw parse_error( "Expected priority", alex.peek() );
    ptr->priority = std::atoi(alex.next().lexeme.c_str());


    for( char c : ptr->format )
        if( c == 'f' )
            ptr->names.emplace_back( std::make_unique<OperatorName>(alex.next()) );
        else
            ptr->names.emplace_back( std::move(parse_variable( alex )) );

    ptr->body = parse_body( alex );
    if( alex.peek().id == '}' )
        throw parse_error( "Right brace never opened", alex.peek() );
    if( alex.has_next() && !Token::declarator(alex.peek()) )
        throw parse_error( "Unfinished operator body", alex.peek() );
    return std::move( ptr );
}

std::unique_ptr<OperatorVariable> parse_variable( Lexer& alex ) {
    if( alex.peek().id == Token::NUM )
        return std::make_unique<NumberVariable>(alex.next());
    if( alex.peek().id == Token::IDENTIFIER )
        return std::make_unique<NamedVariable>(alex.next());
    if( alex.peek().id == Token::STRING )
        return string_to<PairVariable>(alex.next());
    if( alex.peek().id != '{' )
        throw parse_error( "Variable definitions must begin with left brace", alex.peek() );

    Token open_brace = alex.next();

    std::unique_ptr<OperatorVariable> lookahead; // Precisamos de um pouco de lookahead aqui

    if( alex.peek().id == '{' )
        lookahead = parse_variable( alex );
    else {
        Token tok;
        if( !Token::sequence(alex.peek()) )
            throw parse_error( "Expected string, number or identifier after opening brace", alex.peek() );

        tok = alex.next();
        if( alex.peek().id == '}' ) {
            alex.next();
            if( tok.id == Token::IDENTIFIER )
                return std::make_unique<RestrictedVariable>( tok );
            throw parse_error( "Number variables need not be further restricted", tok );
        }
        if( tok.id == Token::NUM )
            lookahead = std::make_unique<NumberVariable>( tok );
        else if( tok.id == Token::IDENTIFIER )
            lookahead = std::make_unique<NamedVariable>( tok );
        else
            lookahead = string_to<PairVariable>( tok );
    }
    if( alex.peek().id != ',' )
        throw parse_error( "Expected either comma or closing brace"
                           " after identifier inside variable", alex.peek() );
    alex.next();
    std::unique_ptr<OperatorVariable> ptr = std::make_unique<PairVariable>(
                                    std::move(lookahead), parse_variable_list(alex) );

    if( alex.peek().id != '}' )
        throw parse_error( "Left brace never closed", open_brace );

    alex.next();
    return std::move(ptr);
}

std::unique_ptr<OperatorVariable> parse_variable_list( Lexer& alex ) {
    std::unique_ptr<OperatorVariable> ptr = parse_variable( alex );

    if( alex.peek().id != ',' ) {
        if( alex.peek().id == '}' )
            return std::move( ptr );
        else
            throw parse_error( "Expected either a comma or a closing brace", alex.peek() );
    }
    alex.next();
    return std::make_unique<PairVariable>( std::move(ptr), parse_variable_list(alex) );
}

std::unique_ptr<OperatorBody> parse_body( Lexer& alex ) {
    auto ptr = std::make_unique<SequenceBody>();
    while( true ) {
        if( Token::sequence(alex.peek()) ) {
            if( alex.peek().id == Token::STRING )
                ptr->sequence.emplace_back( string_to<PairBody>(alex.next()) );
            else
                ptr->sequence.emplace_back( std::make_unique<TerminalBody>(alex.next()) );
        }
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
