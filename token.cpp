/* token.cpp
 * Implementation of token.h.
 */
#include "token.h"

std::ostream& operator<<( std::ostream& os, const Token& tok ) {
    return os << tok.lexeme;
}
