/* token.h
 * Struct that contains all the information pertinent to
 * a token of the language.
 */
#include <string>

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

        IDENTATION = '\t',
        COMMENTARY = '#',
        BLANK      = ' ',
        NEWLINE    = '\n',

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
            case IDENTATION: return "IDENTATION";
            case COMMENTARY: return "COMMENTARY";
            case BLANK:      return "BLANK";
            case NEWLINE:    return "NEWLINE";
            default:         return "unknown token";
        }
    }
};
