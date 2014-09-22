/* either_helper.h
 * Helper classes to template class either.
 */
#ifndef EITHER_BASE_H
#define EITHER_BASE_H

#include <stdexcept>
#include "utility/type_traits.h"
#include "utility/metaprogramming.hpp"

namespace either_helper {

    template< typename T >
    void copy_construct( void * target, const void * source ) {
        new (target) T( *(const T*) source );
    }
    template< typename T >
        void move_construct( void * target, void * source ) {
        new (target) T( std::move(*(T*) source) );
    }
    template< typename T >
        void copy_assign( void * target, const void * source ) {
        *(T*)target = *(const T*)source;
    }
    template< typename T >
        void move_assign( void * target, void * source ) {
        *(T*)target = std::move(*(T*)source);
    }
    template< typename T >
    void destroy( void * target ) {
        ((T*)target)->~T();
    }

    template< typename ... Ts >
    union common_alignment; // substitute for aligned_union_t
    template<>
    union common_alignment<> {
    };
    template< typename Head, typename ... Tail >
    union common_alignment< Head, Tail... > {
        Head head;
        common_alignment<Tail...> tail;
    };

}
#endif // EITHER_BASE_H
