#include <cstring>
#include <iostream>
#include "lexer.h"
#include "parser.h"
#include "semantic_analyser.h"

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
    Parser parser( filename );
    while( parser.has_next() )
        try {
            std::cout << *parser.next() << '\n';
        } catch ( parse_error& ex ) {
            parser.panic();
            std::cerr << ex.what() << ' ' << ex.where.line << ':' << ex.where.column << '\n';
        }
}

void semantic_analysis( const char * filename ) {
    auto parser_ptr = std::make_unique<Parser>( filename );
    SemanticAnalyser semantic_analyser;
    semantic_analyser.set_parser( std::move(parser_ptr) );
    while( semantic_analyser.has_next() )
        try {
            std::cout << *semantic_analyser.next() << '\n';
        } catch ( parse_error& ex ) {
            std::cerr << "Syntagtic error: " << ex.what()
                      << ' ' << ex.where.line << ':' << ex.where.column << '\n';
        } catch ( semantic_error & ex ) {
            std::cerr << "Semantic error: " << ex.what() << '\n';
        }
}

int main( int argc, char * argv[] ) {
    if( argc == 2 ) {
        if( strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0 ) {
            std::cout << "Usage: " << argv[0] << " [-l | -p | -s] <filename>\n"
                         "\n"
                         "This program will analyse the program specified in the last argument.\n"
                         "  -l, --lexer     Do lexical analysis on the program.\n"
                         "  -p, --parser    Do syntactic analysis on the program.\n"
                         "  -s, --semantic  Do semantical analysis on the program. This is the default.\n"
                         "  -h, --help      Display this help and quit.\n";
            return 0;
        }
        semantic_analysis( argv[1] );
        return 0;
    }

    if( argc != 3 ) {
        std::cout << "Usage: " << argv[0] << " [-l | -p | -s] <filename>\n";
        return 1;
    }

    const char * filename = argv[2];
    if( strcmp(argv[1], "-l" ) == 0 || strcmp(argv[1], "--lexer") == 0 ) {
        lexical_analysis( filename );
        return 0;
    }
    if( strcmp(argv[1], "-p" ) == 0 || strcmp(argv[1], "--parser") == 0 ) {
        syntactic_analysis( filename );
        return 0;
    }
    if( strcmp(argv[1], "-s" ) == 0 || strcmp(argv[1], "--semantic") == 0 ) {
        semantic_analysis( filename );
        return 0;
    }


    std::cerr << "Unknown option " << argv[1] << '\n';
    return 1;
}

