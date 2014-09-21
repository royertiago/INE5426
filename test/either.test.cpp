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
        REQUIRE( e.get_as<int>() == 0 );
        REQUIRE_THROWS_AS( e.get_as<char>(), std::runtime_error );

        SECTION( "Assignment" ) {
            e = 7;
            REQUIRE( e.is<int>() );
            REQUIRE_FALSE( e.is<char>() );
            REQUIRE( e.get_as<int>() == 7 );
            REQUIRE_THROWS_AS( e.get_as<char>(), std::runtime_error );

            int i = e;
            REQUIRE( i == 7 );

            SECTION( "Type change through assignment" ) {
                e = '7';
                REQUIRE_FALSE( e.is<int>() );
                REQUIRE( e.is<char>() );
                REQUIRE_THROWS_AS( e.get_as<int>(), std::runtime_error );
                REQUIRE( e.get_as<char>() == '7' );

                char c = e;
                REQUIRE( c == '7' );
            }
        }
    }

    SECTION( "Assignment constructor" ) {
        either< int, char > p = (int) 'A';
        REQUIRE( p.is<int>() );
        REQUIRE_FALSE( p.is<char>() );
        REQUIRE( p.get_as<int>() == (int) 'A' );
        REQUIRE_THROWS_AS( p.get_as<char>(), std::runtime_error );

        either< int, char > e = 'A';
        REQUIRE_FALSE( e.is<int>() );
        REQUIRE( e.is<char>() );
        REQUIRE_THROWS_AS( e.get_as<int>(), std::runtime_error );
        REQUIRE( e.get_as<char>() == 'A' );

        SECTION( "Type change through assignment" ) {
            e = '7';
            REQUIRE_FALSE( e.is<int>() );
            REQUIRE( e.is<char>() );
            REQUIRE_THROWS_AS( e.get_as<int>(), std::runtime_error );
            REQUIRE( e.get_as<char>() == '7' );

            char c = e;
            REQUIRE( c == '7' );
        }

    }

    SECTION( "Copy" ) {
        either< int, char > e = '7';

        either< int, char > f = e;
        REQUIRE_FALSE( f.is<int>() );
        REQUIRE( f.is<char>() );
        REQUIRE_THROWS_AS( f.get_as<int>(), std::runtime_error );
        REQUIRE( f.get_as<char>() == '7' );
        char c = f;
        REQUIRE( c == '7' );
    }

    SECTION( "Relational operators" ) {
        either< int, char > e = '7';
        either< int, char > f = e;

        REQUIRE( e == f );
        REQUIRE_FALSE( e < f );
        REQUIRE_FALSE( f < e );
        e = '8';
        REQUIRE_FALSE( e == f );
        REQUIRE_FALSE( e < f );
        REQUIRE( f < e );
        f = 800;
        REQUIRE_FALSE( e == f );
        REQUIRE_FALSE( e < f );
        REQUIRE( f < e );
        e = 801;
        REQUIRE_FALSE( e == f );
        REQUIRE( f < e );
        REQUIRE_FALSE( e < f );
    }

    SECTION( "Homogeneous case" ) {
        either< int, int > e;
        REQUIRE( e.is<int>() );
        e = 7;
        REQUIRE( e.is<int>() );
        REQUIRE( e.get_as<int>() == 7 );
        either< int, int > f = e;
        REQUIRE( e == f );
        REQUIRE_FALSE( e < f );
        REQUIRE_FALSE( f < e );
        f = 14;
        REQUIRE_FALSE( e == f );
        REQUIRE( e < f );
        REQUIRE_FALSE( f < e );
        e = 21;
        REQUIRE_FALSE( e == f );
        REQUIRE_FALSE( e < f );
        REQUIRE( f < e );
    }

    SECTION( "Container use - std::vector" ) {
        std::vector< either< int, char > > v;
        either< int, char > e = 0;
        v.resize( 5 );
        REQUIRE( v.size() == 5 );
        for( auto& x : v )
            REQUIRE( x == e );
        v = {1, 5, 'd', 45, 'a', 'b', 'z', 879};
        e = 'a';
        REQUIRE( e == v[4] );
        e = 45;
        REQUIRE( e == v[3] );
        REQUIRE_FALSE( e == v[4] );

        SECTION( "Realocation" ) {
            std::vector< either<int, char> > w = v;
            w.resize( w.capacity() + 1 );
            w.resize( v.size() );
            for( auto it = w.begin(), 
                      jt = v.begin();
                      it != w.end();
                      ++it, ++jt )
                REQUIRE( *it == *jt );
        }

        SECTION( "Sorting" ) {
            std::sort( v.begin(), v.end() );
            for( int i = 0; i < 4; ++i ) {
                REQUIRE( v[i].is<int>() );
                REQUIRE_FALSE( v[i].is<char>() );
            }
            for( int i = 4; i < 8; ++i ) {
                REQUIRE_FALSE( v[i].is<int>() );
                REQUIRE( v[i].is<char>() );
            }

            std::vector< either<int, char> > w = 
                    {1, 5, 45, 879, 'a', 'b', 'd', 'z'};
            for( auto it = w.begin(), 
                      jt = v.begin(); 
                      it != w.end(); 
                      ++it, ++jt )
                REQUIRE( *it == *jt );
        }
    }

    SECTION( "Container use - std::map" ) {
        std::map< either< int, char >, bool > m; // m[int] == true
        m.insert({either<int, char>( 'A' ), true});
        m.insert({either<int, char>( 65 ), false});

        for( const auto& pair : m )
            REQUIRE( pair.first.is<char>() == pair.second );

        auto result = m.insert({either<int, char>( 'A' ), false});
        REQUIRE_FALSE( result.second );

        result = m.insert({either<int, char>( 65 ), true});
        REQUIRE_FALSE( result.second );

        m.insert({ 'B', true });
        m.insert({ 66, false });
        REQUIRE( m.size() == 4 );

        for( const auto& pair : m )
            REQUIRE( pair.first.is<char>() == pair.second );

        m.at( 'A' ) = false;
        m.at( 'B' ) = false;
        m.at( 65 ) = true;
        m.at( 66 ) = true;

        for( const auto& pair : m ) // não é mais is<char>
            REQUIRE( pair.first.is<int>() == pair.second );

        m['A'] = true;
        m['B'] = true;
        m['C'] = true;
        m[ 65 ] = false;
        m[ 66 ] = false;
        m[ 67 ] = false;
        REQUIRE( m.size() == 6 );
        for( const auto& pair : m )
            REQUIRE( pair.first.is<char>() == pair.second );
    }

    SECTION( "Container use - std::map - homogeneous case" ) {
        std::map< either<double, double, double>, bool > n;
        n.insert({either<double, double, double>(1.0), true});
        n.insert({either<double, double, double>(-1.0), false});

        for( const auto& pair : n )
            REQUIRE( (pair.first.get_as<double>() > 0) == pair.second );

        auto result = n.insert({either<double, double, double>(1.0), true});
        REQUIRE_FALSE( result.second );

        result = n.insert({either<double, double, double>(-1.0), false});
        REQUIRE_FALSE( result.second );
        
        for( const auto& pair : n )
            REQUIRE( (pair.first.get_as<double>() > 0) == pair.second );

        n.insert({ 2.0, true });
        n.insert({ -2.0, false });
        REQUIRE( n.size() == 4 );

        for( const auto& pair : n )
            REQUIRE( (pair.first.get_as<double>() > 0) == pair.second );

        n.at( 1.0 ) = false;
        n.at( 2.0 ) = false;
        n.at( -1.0 ) = true;
        n.at( -2.0 ) = true;

        for( const auto& pair : n )
            REQUIRE( (pair.first.get_as<double>() < 0) == pair.second );

        n[1.0] = true;
        n[2.0] = true;
        n[3.0] = true;
        n[-1.0] = false;
        n[-2.0] = false;
        n[-3.0] = false;
        REQUIRE( n.size() == 6 );
        for( const auto& pair : n )
            REQUIRE( (pair.first.get_as<double>() > 0) == pair.second );

        std::vector< either<double, double, double> > vd;
        vd.clear();
        for( const auto& pair : n )
            vd.insert( vd.begin(), pair.first ); // testar movimento

        std::vector< either<double, double, double>> wd
            = {3.0, 2.0, 1.0, -1.0, -2.0, -3.0};
        for( auto it = wd.begin(), jt = vd.begin(); it != wd.end(); ++it, ++jt )
            REQUIRE( *it == *jt );
    }

    SECTION( "Complex resources" ) {
        { //subscope
            Resource::reset();

            Resource r( 17 );
            either< Resource, int > e = std::move( r );
            REQUIRE( Resource::copy_count() == 0 );

            either< Resource, int > f = 7;
            f = std::move( e );
            REQUIRE( Resource::copy_count() == 0 );

            either< Resource, int > g = Resource::make();
            REQUIRE( Resource::copy_count() == 0 );

            g = f = 7;
            f = get_either_resource();
        }
        REQUIRE( Resource::copy_count() == 0 );
        REQUIRE( Resource::alive_count() == 0 );
    }
}
