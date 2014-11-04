/* token.cpp
 * Implementation of token.h.
 */
#include "token.h"
#include "utility/metaoperators.hpp"

std::ostream& operator<<( std::ostream& os, const Token& tok ) {
    return os << tok.lexeme;
}

bool operator==( const Token & lhs, const Token & rhs ) {
    return mp::equal_to( lhs, rhs, &Token::id, &Token::lexeme );
}

bool operator!=( const Token & lhs, const Token & rhs ) {
    return mp::not_equal_to( lhs, rhs, &Token::id, &Token::lexeme );
}

bool operator<=( const Token & lhs, const Token & rhs ) {
    return mp::less_equal( lhs, rhs, &Token::id, &Token::lexeme );
}

bool operator>=( const Token & lhs, const Token & rhs ) {
    return mp::greater_equal( lhs, rhs, &Token::id, &Token::lexeme );
}

bool operator< ( const Token & lhs, const Token & rhs ) {
    return mp::less( lhs, rhs, &Token::id, &Token::lexeme );
}

bool operator> ( const Token & lhs, const Token & rhs ) {
    return mp::greater( lhs, rhs, &Token::id, &Token::lexeme );
}
