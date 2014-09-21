/* metaprogramming.test.cpp
 * Unit test to metafunctions from utility/metaprogramming.hpp
 */
#include "utility/metaprogramming.hpp"

#include "catch.hpp"

TEST_CASE( "logical_and metafunction", "[mp]" ) {
    using mp::logical_and;

    SECTION( "Nullary" ) {
        REQUIRE( logical_and<>::value == true );
    }
    SECTION( "Unary" ) {
        REQUIRE( logical_and<true>::value == true );
        REQUIRE( logical_and<false>::value == false );
    }
    SECTION( "Binary" ) {
        REQUIRE( (logical_and<true, true >::value) == true );
        REQUIRE( (logical_and<false, true >::value) == false );
        REQUIRE( (logical_and<true, false>::value) == false );
        REQUIRE( (logical_and<false, false>::value) == false );
    }
    SECTION( "Quinary" ) {
        REQUIRE( (logical_and<true, true, true, true, true>::value) == true );
        REQUIRE( (logical_and<true, true, true, false, true>::value) == false );
        REQUIRE( (logical_and<true, false, true, true, true>::value) == false );
    }
}

TEST_CASE( "logical_or metafunction", "[mp]" ) {
    using mp::logical_or;

    SECTION( "Nullary" ) {
        REQUIRE( logical_or<>::value == false );
    }
    SECTION( "Unary" ) {
        REQUIRE( logical_or<true>::value == true );
        REQUIRE( logical_or<false>::value == false );
    }
    SECTION( "Binary" ) {
        REQUIRE( (logical_or<true, true >::value) == true );
        REQUIRE( (logical_or<false, true >::value) == true );
        REQUIRE( (logical_or<true, false>::value) == true );
        REQUIRE( (logical_or<false, false>::value) == false );
    }
    SECTION( "Quinary" ) {
        REQUIRE( (logical_or<false, true, false, false, true>::value) == true );
        REQUIRE( (logical_or<true, true, true, true, true>::value) == true );
        REQUIRE( (logical_or<false, false, false, false, false>::value) == false );
    }
}
