/* token.h
 * Struct that contains all the information pertinent to
 * a token of the language.
 *
 * Tokens are comparable - they form a total order. The comparison
 * is done firstly by comparing the IDs and then by comparing the lexeme.
 * The position of the token is irrelevant in such comparison;
 * the ordering exists merely to imposes a total ordering in this class.
 */
#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <ostream>

struct Token {
    enum {
        NUM = 1,
        INCLUDE,
        CATEGORY,

        IDENTIFIER,
        STRING,

        LBRACE = '{',
        COMMA  = ',',
        RBRACE = '}',

        F,
        FX,
        FY,
        XF,
        YF,
        XFX,
        YFX,
        XFY,

        UNKNOWN = ~0
    };
    // Token id; it is always one of the above
    unsigned id;

    // Token lexeme: actual string that produced the token
    std::string lexeme;

    // Source line and column of the token
    std::size_t line, column;

    // String representation of the token ID.
    const char * c_str() const {
        switch( id ) {
            case NUM:        return "NUM";
            case INCLUDE:    return "INCLUDE";
            case CATEGORY:   return "CATEGORY";
            case F:          return "F";
            case FX:         return "FX";
            case FY:         return "FY";
            case XF:         return "XF";
            case YF:         return "YF";
            case XFX:        return "XFX";
            case YFX:        return "YFX";
            case XFY:        return "XFY";
            case IDENTIFIER: return "IDENTIFIER";
            case STRING:     return "STRING";
            case LBRACE:     return "LBRACE";
            case COMMA:      return "COMMA";
            case RBRACE:     return "RBRACE";
            default:         return "unknown token";
        }
    }

    /* Useful queries about tokens */

    /* Declarator: a token that can start a declaration. */
    static bool declarator( const Token & tok ) {
        switch( tok.id ) {
            case INCLUDE:
            case CATEGORY:
            case F:
            case FX:
            case FY:
            case XF:
            case YF:
            case XFX:
            case YFX:
            case XFY:
                return true;
            default: return false;
        }
    }

    /* Sequence: tokens that can appear in sequence in a operator body. */
    static bool sequence( const Token & tok ) {
        switch( tok.id ) {
            case NUM:
            case IDENTIFIER:
            case STRING:
                return true;
            default: return false;
        }
    }
};

// ostream printing
std::ostream& operator<<( std::ostream&, const Token& );

// Relational operators
bool operator==( const Token&, const Token& );
bool operator!=( const Token&, const Token& );
bool operator<=( const Token&, const Token& );
bool operator>=( const Token&, const Token& );
bool operator< ( const Token&, const Token& );
bool operator> ( const Token&, const Token& );

#endif // TOKEN_H
