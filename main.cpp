#include <iostream>
#include <fstream>

#include <iterator>
#include "lexertl/generator.hpp"
#include "lexertl/stream_shared_iterator.hpp"
#include "lexertl/iterator.hpp"
#include "lexertl/lookup.hpp"
#include "lexertl/debug.hpp"
#include "lexertl/generate_cpp.hpp"

enum Token {
    NUM = 1,
    INCLUDE,
    CATEGORY,
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
};

const char * token_to_string( long unsigned int t ) {
    switch( t ) {
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
int main( int argc, char * argv[] ) {
    if( argc != 2 ) {
        std::cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    std::ifstream ifi( argv[1] );
    lexertl::stream_shared_iterator iter(ifi);
    lexertl::stream_shared_iterator end;
    lexertl::match_results<lexertl::stream_shared_iterator>
                results(iter, end);


    lexertl::rules rules;
    lexertl::state_machine sm;

    rules.push( "^include",           INCLUDE);
    rules.push( "^(class|category)",  CATEGORY );
    rules.push( "^xfx",               XFX );
    rules.push( "^xfy",               XFY );
    rules.push( "^yfx",               YFX );
    rules.push( "^xf",                XF );
    rules.push( "^yf",                YF );
    rules.push( "^fx",                FX );
    rules.push( "^fy",                FY );
    rules.push( "^f",                 F );
    rules.push( "^(    |\t)",         IDENTATION );
    rules.push( "( |\t)+",            BLANK );
    rules.push( "\n",                 NEWLINE );
    rules.push( "\\d+",               NUM );
    rules.push( "[^ \t\n\r\f\v{},]+", STRING );
    rules.push( "\\{",                LBRACE );
    rules.push( "\\}",                RBRACE );
    rules.push( ",",                  COMMA );
    rules.push( "^("
                    "[^icxyf]"
                    "|i[^n]"
                    "|in[^c]"
                    "|inc[^l]"
                    "|incl[^u]"
                    "|inclu[^d]"
                    "|includ[^e]"
                    "|include\\S"
                    "|c[^al]"
                    "|cl[^a]"
                    "|cla[^s]"
                    "|clas[^s]"
                    "|class\\S"
                    "|ca[^t]"
                    "|cat[^e]"
                    "|cate[^g]"
                    "|categ[^o]"
                    "|catego[^r]"
                    "|categor[^y]"
                    "|category\\S"
                    "|x[^f]"
                    "|xf[^ yx\t\n\r\f\v]"
                    "|xfx\\S"
                    "|xfy\\S"
                    "|y[^f]"
                    "|yf[^ x\t\n\r\f\v]"
                    "|yfx\\S"
                    "|f[^ yx\t\n\r\f\v]"
                    "|fx\\S"
                    "|fy\\S"
                 ").*",              COMMENTARY );

    lexertl::generator::build( rules, sm );

    lexertl::lookup(sm, results);

    while (results.id != 0)
    {
        std::cout << "Id: " << token_to_string(results.id) << ", Token: '" <<
            results.str() << "'\n";
        lexertl::lookup(sm, results);
    }

    return 0;
}

