/* either.test.cpp
 * Unit test to 'either' template class.
 */
#include "utility/either.h"

#include <algorithm> // sort
#include <map>
#include <tuple>
#include <vector>

#include "test/resource.h"
#include "catch.hpp"

namespace {

either< Resource, int > get_either_resource() {
    return Resource( 23 );
}

} // anonymous namespace


TEST_CASE( "Basic either funcionality", "[either]" ) {

    SECTION( "Default constructor" ) {
        either< int, char > e;
        REQUIRE( e.is<int>() );
        REQUIRE_FALSE( e.is<char>() );
        REQUIRE( e.get<int>() == 0 );
        REQUIRE_THROWS_AS( e.get<char>(), std::runtime_error );

        SECTION( "Assignment" ) {
            e = 7;
            REQUIRE( e.is<int>() );
            REQUIRE_FALSE( e.is<char>() );
            REQUIRE( e.get<int>() == 7 );
            REQUIRE_THROWS_AS( e.get<char>(), std::runtime_error );

            int i = e;
            REQUIRE( i == 7 );

            SECTION( "Type change through assignment" ) {
                e = '7';
                REQUIRE_FALSE( e.is<int>() );
                REQUIRE( e.is<char>() );
                REQUIRE_THROWS_AS( e.get<int>(), std::runtime_error );
                REQUIRE( e.get<char>() == '7' );

                char c = e;
                REQUIRE( c == '7' );
            }
        }
    }

    SECTION( "Assignment constructor" ) {
        either< int, char > p = (int) 'A';
        REQUIRE( p.is<int>() );
        REQUIRE_FALSE( p.is<char>() );
        REQUIRE( p.get<int>() == (int) 'A' );
        REQUIRE_THROWS_AS( p.get<char>(), std::runtime_error );

        either< int, char > e = 'A';
        REQUIRE_FALSE( e.is<int>() );
        REQUIRE( e.is<char>() );
        REQUIRE_THROWS_AS( e.get<int>(), std::runtime_error );
        REQUIRE( e.get<char>() == 'A' );

        SECTION( "Type change through assignment" ) {
            e = '7';
            REQUIRE_FALSE( e.is<int>() );
            REQUIRE( e.is<char>() );
            REQUIRE_THROWS_AS( e.get<int>(), std::runtime_error );
            REQUIRE( e.get<char>() == '7' );

            char c = e;
            REQUIRE( c == '7' );
        }

    }

    SECTION( "Copy" ) {
        either< int, char > e = '7';

        either< int, char > f = e;
        REQUIRE_FALSE( f.is<int>() );
        REQUIRE( f.is<char>() );
        REQUIRE_THROWS_AS( f.get<int>(), std::runtime_error );
        REQUIRE( f.get<char>() == '7' );
        char c = f;
        REQUIRE( c == '7' );
    }

    SECTION( "Homogeneous case" ) {
        either< int, int > e;
        e = 7;
        REQUIRE( e.is<int>() );
        REQUIRE( e.get<int>() == 7 );
        either< int, int > f = e;
        REQUIRE( f.is<int>() );
        REQUIRE( f.get<int>() == 7 );
        f = 14;
        REQUIRE( f.get<int>() == 14 );
    }

    SECTION( "Complex resources" ) {
        { //subscope
            Resource::reset();

            Resource r( 17 );
            either< Resource, int > e = std::move( r );
            REQUIRE( e.is<Resource>() );

            either< Resource, int > f = 7;
            f = std::move( e );
            REQUIRE( f.is<Resource>() );
            REQUIRE( Resource::alive_count() == 1 );

            either< Resource, int > g = Resource::make();
            REQUIRE( Resource::alive_count() == 2 );

            g = f = 7;
            REQUIRE( Resource::alive_count() == 0 );
            f = get_either_resource();
            REQUIRE( Resource::alive_count() == 1 );
        }
        REQUIRE( Resource::copy_count() == 0 );
        REQUIRE( Resource::alive_count() == 0 );
    }
}
