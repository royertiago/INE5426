#include <cstdio>
#include <lexertl/generator.hpp>
#include <lexertl/lookup.hpp>

enum Token {
    INTEGER = 1,
    WORD
};

int main() {
    lexertl::rules rules;
    lexertl::state_machine automata;

    rules.push( "[0-9]+", INTEGER );
    rules.push( "[a-z]+", WORD );
    lexertl::generator::build( rules, automata );

    char input[] = "aa111aaAaa";
    lexertl::match_results<char*> results( input, input + sizeof(input) );

    for( lexertl::lookup(automata, results); results.id != 0; lexertl::lookup(automata, results) )
        printf( "Id: %lu - token: '%s'\n", results.id, results.str().c_str() );

    return 0;
}
