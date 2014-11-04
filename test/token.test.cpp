/* token.test.cpp
 * Unit test of the relational operators in Token struct.
 */
#include "token.h"
#include <catch.hpp>

TEST_CASE( "Token relational operators", "[Token][relational]" ) {
   Token a{Token::NUM, "TOK", 1, 1}, b(a);

   SECTION( "a == b" ) {
       CHECK      ( a == b );
       CHECK      ( a <= b );
       CHECK      ( a >= b );
       CHECK_FALSE( a != b );
       CHECK_FALSE( a <  b );
       CHECK_FALSE( a >  b );
   }

   SECTION( "a > b" ) {
       SECTION( "difference on first attribute" ) {
           a.id++;

           CHECK_FALSE( a == b );
           CHECK_FALSE( a <= b );
           CHECK      ( a >= b );
           CHECK      ( a != b );
           CHECK_FALSE( a <  b );
           CHECK      ( a >  b );
       }

       SECTION( "difference on second attribute" ) {
           a.lexeme[2]++;

           CHECK_FALSE( a == b );
           CHECK_FALSE( a <= b );
           CHECK      ( a >= b );
           CHECK      ( a != b );
           CHECK_FALSE( a <  b );
           CHECK      ( a >  b );
       }

       SECTION( "difference in both attributes" ) {
           a.id++;
           a.lexeme[2]++;

           CHECK_FALSE( a == b );
           CHECK_FALSE( a <= b );
           CHECK      ( a >= b );
           CHECK      ( a != b );
           CHECK_FALSE( a <  b );
           CHECK      ( a >  b );
       }
   }

   SECTION( "a < b" ) {
       SECTION( "difference on first attribute" ) {
           b.id++;

           CHECK_FALSE( a == b );
           CHECK      ( a <= b );
           CHECK_FALSE( a >= b );
           CHECK      ( a != b );
           CHECK      ( a <  b );
           CHECK_FALSE( a >  b );
       }

       SECTION( "difference on second attribute" ) {
           b.lexeme[2]++;

           CHECK_FALSE( a == b );
           CHECK      ( a <= b );
           CHECK_FALSE( a >= b );
           CHECK      ( a != b );
           CHECK      ( a <  b );
           CHECK_FALSE( a >  b );
       }

       SECTION( "difference in both attributes" ) {
           b.id++;
           b.lexeme[2]++;

           CHECK_FALSE( a == b );
           CHECK      ( a <= b );
           CHECK_FALSE( a >= b );
           CHECK      ( a != b );
           CHECK      ( a <  b );
           CHECK_FALSE( a >  b );
       }
   }
}
