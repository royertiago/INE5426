#include <iostream>
#include "lexer.h"

int main( int argc, char * argv[] ) {
    if( argc != 2 ) {
        std::cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }
    Lexer alex( argv[1] );

    while( alex.has_next() )
    {
        auto tok = alex.next();
        std::cout << "Id: " << tok.c_str() << ", Lexeme: '" <<
            tok.lexeme << "' - " << tok.line << ':' << tok.column << '\n';
    }

    return 0;
}

