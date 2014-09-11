/* lexer.cpp
 * Implementation of lexer.h
 */
#include "lexer.h"

lexertl::state_machine Lexer::sm;

void Lexer::init() {
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
