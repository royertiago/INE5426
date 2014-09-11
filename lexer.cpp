/* lexer.cpp
 * Implementation of lexer.h
 */
#include <fstream>
#include <lexertl/generator.hpp>
#include <lexertl/lookup.hpp>

#include "lexer.h"

namespace {
    lexertl::state_machine sm;

    void init() {
        static bool inited = false;
        if( inited ) return;
        inited = true;

        lexertl::rules rules;

        rules.push( "^include",                  Token::INCLUDE);
        rules.push( "^(class|category)",         Token::CATEGORY );
        rules.push( "^xfx",                      Token::XFX );
        rules.push( "^xfy",                      Token::XFY );
        rules.push( "^yfx",                      Token::YFX );
        rules.push( "^xf",                       Token::XF );
        rules.push( "^yf",                       Token::YF );
        rules.push( "^fx",                       Token::FX );
        rules.push( "^fy",                       Token::FY );
        rules.push( "^f",                        Token::F );
        rules.push( "^("
                        "[^icxyf \t\n\r\f\v]"
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
                    ").*",                       rules.skip() );
        rules.push( "( |\t)+",                   rules.skip() );
        rules.push( "\n",                        rules.skip() );
        rules.push( "\\d+",                      Token::NUM );
        rules.push( "\\{",                       Token::LBRACE );
        rules.push( "\\}",                       Token::RBRACE );
        rules.push( ",",                         Token::COMMA );
        rules.push( "\\\"([^\"]|\\\\\\\")*\\\"", Token::STRING );
        rules.push( "[^ \t\n\r\f\v{},]+",        Token::IDENTIFIER );

        lexertl::generator::build( rules, sm );
    }

} // anonymous namespace

void Lexer::compute_next() {
    lexertl::lookup(sm, _results);
    _next.id = _results.id;
    _next.lexeme = _results.str();
    _next.line = _results.start.line();
    _next.column = _results.start.column();
}

Lexer::Lexer( const char * filename ) :
    _data( new std::string )
{
    init();
    std::ifstream in( filename, std::ios::in );
    if( !in ) {
        throw "Read error";
    }
    in.seekg( 0, std::ios::end );
    _data->resize( in.tellg() );
    in.seekg( 0, std::ios::beg );
    in.read( &(*_data)[0], _data->size() );
    in.close();

    position_iterator<std::string::iterator> iter( _data->begin() );
    position_iterator<std::string::iterator> end( _data->end() );

    _results = lexertl::match_results<decltype(iter)>( iter, end );

    compute_next();
}
