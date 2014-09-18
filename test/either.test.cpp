/* either.test.cpp
 * Unit test to 'either' template class.
 * TODO: rewrite names to conform new naming convention
 * TODO: rewrite test to conform Catch's more organized style
 */
#include "utility/either.h"

#include <algorithm> // sort
#include <map>
#include <tuple>
#include <vector>

#include "test/resource.h"
#include "catch.hpp"

namespace {
// Retorns true if passed value coincides with t's value.
bool eitherForwardTest( std::tuple<either<int, char>> t, char c ) {
    return std::get<0>(t).is<char>() && std::get<0>(t).get_as<char>() == c;
}
bool eitherForwardTest( std::tuple<either<int, char>> t, int i ) {
    return std::get<0>(t).is<int>() && std::get<0>(t).get_as<int>() == i;
}

either<double, double, double> getEitherDouble() {
    return either<double, double, double>( 3.5 );
}

either< Resource, int > getEitherResource() {
    return Resource( 23 );
}

} // anonymous namespace


TEST_CASE( "either Test", "[either]" ) {

    // Construtor padrão
    either< int, char > e;

    REQUIRE( e.is<int>() );
    REQUIRE_FALSE( e.is<char>() );
    REQUIRE( e.get_as<int>() == 0 );
    REQUIRE_THROWS_AS( e.get_as<char>(), std::runtime_error );

    // Construtor de atribuição
    either< int, char > p = 'A';
    REQUIRE_FALSE( p.is<int>() );
    REQUIRE( p.is<char>() );
    REQUIRE_THROWS_AS( p.get_as<int>(), std::runtime_error );
    REQUIRE( p.get_as<char>() == 'A' );

    either< int, char > q = (int) 'A';
    REQUIRE( q.is<int>() );
    REQUIRE_FALSE( q.is<char>() );
    REQUIRE( q.get_as<int>() == (int) 'A' );
    REQUIRE_THROWS_AS( q.get_as<char>(), std::runtime_error );

    // Atribuição
    e = 7;
    REQUIRE( e.is<int>() );
    REQUIRE_FALSE( e.is<char>() );
    REQUIRE( e.get_as<int>() == 7 );
    REQUIRE_THROWS_AS( e.get_as<char>(), std::runtime_error );

    int i = e;
    REQUIRE( i == 7 );

    // Alteração de tipo
    e = '7';
    REQUIRE_FALSE( e.is<int>() );
    REQUIRE( e.is<char>() );
    REQUIRE_THROWS_AS( e.get_as<int>(), std::runtime_error );
    REQUIRE( e.get_as<char>() == '7' );

    char c = e;
    REQUIRE( c == '7' );

    // Cópia
    either< int, char > f = e;
    REQUIRE_FALSE( f.is<int>() );
    REQUIRE( f.is<char>() );
    REQUIRE_THROWS_AS( f.get_as<int>(), std::runtime_error );
    REQUIRE( f.get_as<char>() == '7' );
    c = f;
    REQUIRE( c == '7' );

    // Operadores relacionais
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

    // Homogêneo
    either< int, int > j;
    REQUIRE( j.is<int>() );
    j = 7;
    REQUIRE( j.is<int>() );
    REQUIRE( j.get_as<int>() == 7 );
    either< int, int > k = j;
    REQUIRE( j == k );
    REQUIRE_FALSE( j < k );
    REQUIRE_FALSE( k < j );
    k = 14;
    REQUIRE_FALSE( j == k );
    REQUIRE( j < k );
    REQUIRE_FALSE( k < j );
    j = 21;
    REQUIRE_FALSE( j == k );
    REQUIRE_FALSE( j < k );
    REQUIRE( k < j );

    // forward_as_tuple

    REQUIRE( eitherForwardTest( 
                std::forward_as_tuple( either<int, char>('a') ), 
                'a') );
    REQUIRE_FALSE( eitherForwardTest( 
                std::forward_as_tuple( std::move(either<int, char>('b')) ),
                'a') );
    REQUIRE_FALSE( eitherForwardTest(
                std::forward_as_tuple( std::move( either<int, char>(7)) ),
                'a') );
    REQUIRE( eitherForwardTest(
                std::forward_as_tuple( std::move( either<int, char>(7)) ),
                7) );
    REQUIRE_FALSE( eitherForwardTest(
                std::forward_as_tuple( std::move( either<int, char>(8)) ),
                7) );
    REQUIRE_FALSE( eitherForwardTest(
                std::forward_as_tuple( std::move( either<int, char>(7)) ),
                '7') );

    // Semânticas de movimento
    either<double, double, double> ed = getEitherDouble();
    REQUIRE( ed.get_as<double>() == 3.5 );

    either<double, double, double> ef = std::move(getEitherDouble());
    REQUIRE( ef.get_as<double>() == 3.5 );

    ed = 2.0;
    ed = getEitherDouble();
    REQUIRE( ed.get_as<double>() == 3.5 );

    ef = 2.0;
    ef = std::move( getEitherDouble() );
    REQUIRE( ef.get_as<double>() == 3.5 );

    // Uso em contêiners - std::vector
    std::vector< either< int, char > > v;
    e = 0;
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

    std::vector< either<int, char> > w = v;
    w.resize( w.capacity() + 1 ); // Forçar realocação
    w.resize( v.size() );
    for( auto it = w.begin(), jt = v.begin(); it != w.end(); ++it, ++jt )
        REQUIRE( *it == *jt );

    std::sort( v.begin(), v.end() );
    for( int i = 0; i < 4; ++i ) {
        REQUIRE( v[i].is<int>() );
        REQUIRE_FALSE( v[i].is<char>() );
    }
    for( int i = 4; i < 8; ++i ) {
        REQUIRE_FALSE( v[i].is<int>() );
        REQUIRE( v[i].is<char>() );
    }

    w = {1, 5, 45, 879, 'a', 'b', 'd', 'z'};
    for( auto it = w.begin(), jt = v.begin(); it != w.end(); ++it, ++jt )
        REQUIRE( *it == *jt );

    // Uso em contêiners - std::map
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

    v.clear();
    for( const auto& pair : m )
        v.insert( v.begin(), pair.first ); // testar movimento

    w = {'C', 'B', 'A', 67, 66, 65};
    for( auto it = w.begin(), jt = v.begin(); it != w.end(); ++it, ++jt )
        REQUIRE( *it == *jt );

    // Uso em contêineres - map com either homogêneo
    std::map< either<double, double, double>, bool > n;
    n.insert({either<double, double, double>(1.0), true});
    n.insert({either<double, double, double>(-1.0), false});

    for( const auto& pair : n )
        REQUIRE( (pair.first.get_as<double>() > 0) == pair.second );

    auto resul = n.insert({either<double, double, double>(1.0), true});
    REQUIRE_FALSE( result.second );

    resul = n.insert({either<double, double, double>(-1.0), false});
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

    // Teste com objetos complexos
    {
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
        f = getEitherResource();
        REQUIRE( Resource::copy_count() == 0 );
    }
    REQUIRE( Resource::alive_count() == 0 );
}
