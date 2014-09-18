/* resource.cpp
 * Implementação de resource.h
 */
#include "resource.h"

namespace {
    static unsigned copy_count = 0;
    static unsigned object_count = 0;
}

// Métodos estáticos
unsigned Resource::copy_count() {
    return ::copy_count;
}

unsigned Resource::alive_count() {
    return ::object_count;
}

void Resource::reset() {
    ::copy_count = 0;
    ::object_count = 0;
}

Resource Resource::make() {
    return Resource( 42 );
}

// Construtores
Resource::Resource( unsigned ) :
    valid( true )
{
    ::object_count++;
}

Resource::Resource( const Resource& ) :
    valid( true )
{
    ::copy_count++;
    ::object_count++;
}

Resource::Resource( Resource&& r ) :
    valid( r.valid )
{
    r.valid = false;
}

Resource& Resource::operator=( const Resource& r ) {
    if( valid && !r.valid )
        ::object_count--; // Este objeto também passa a ser inválido
    if( !valid && r.valid )
        ::object_count++; // Este objeto passa a ser válido.

    valid = r.valid;
    return *this;
}

Resource& Resource::operator=( Resource&& r ) {
    bool tmp = valid;
    valid = r.valid;
    r.valid = tmp;
    return *this;
}

Resource::~Resource() {
    if( valid )
        ::object_count--;
}
