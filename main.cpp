#include <iostream>
#include <cstring>
#include "lexer.h"
#include "parser.h"

void lexical_analysis( const char * filename ) {
    Lexer alex( filename );

    while( alex.has_next() )
    {
        auto tok = alex.next();
        std::cout << "Id: " << tok.c_str() << ", Lexeme: '" <<
            tok.lexeme << "' - " << tok.line << ':' << tok.column << '\n';
    }
}

void syntactic_analysis( const char * filename ) {
    try {
        Parser parser( filename );
        while( parser.has_next() )
            std::cout << *parser.next() << '\n';
    } catch ( parse_error& ex ) {
        std::cerr << ex.what() << ' ' << ex.where.line << ':' << ex.where.column << '\n';
    }
}

int main( int argc, char * argv[] ) {
    char * filename;
    bool lexical = false;

    if( argc == 2 )
        filename = argv[1];
    else if( argc == 3 ) {
        filename = argv[2];
        if( strcmp(argv[1], "-l" ) == 0 )
            lexical = true;
        else {
            std::cerr << "Unknown option " << argv[1] << '\n';
            return 1;
        }
    }
    else {
        std::cerr << "Usage: " << argv[0] << " [-l] <filename>\n";
        return 1;
    }

    if( lexical )
        lexical_analysis( filename );
    else
        syntactic_analysis( filename );
    return 0;
}

