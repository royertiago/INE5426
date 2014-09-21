/* either_helper.h
 * Helper classes to template class either.
 */
#ifndef EITHER_BASE_H
#define EITHER_BASE_H

#include <stdexcept>
#include "utility/type_traits.h"
#include "utility/metaprogramming.hpp"

namespace either_helper {

    struct abstract_pseudo_virtual_table {
        virtual void copy_construct( void * target, const void * source ) const = 0;
        virtual void move_construct( void * target, void * source ) const = 0;
        virtual void copy_assign( void * target, const void * source ) const = 0;
        virtual void move_assign( void * target, void * source ) const = 0;
        virtual void destroy( void * target ) const = 0;
        virtual ~abstract_pseudo_virtual_table() = default;
    };

#define AUX_META( function_name,                                                \
                  type_target, name_target,                                     \
                  type_source, name_source,                                     \
                  expression, exception_text )                                  \
    template< typename T, bool >                                                \
    struct function_name {                                                      \
        static void f( type_target name_target, type_source name_source ) {     \
            expression;                                                         \
        }                                                                       \
    };                                                                          \
    template< typename T >                                                      \
    struct function_name<T, false> {                                            \
        static void f( type_target name_target, type_source name_source ) {     \
            throw std::logic_error( exception_text );                           \
        }                                                                       \
    }

    AUX_META( copy_construct, void *, target,
                              const void *, source,
                new (target) T( *(const T*) source ),
                "Type is not copy constructible"
            );
    AUX_META( move_construct, void *, target,
                              void *, source,
                new (target) T( std::move(*(T*) source) ),
                "Type is not copy constructible"
            );
    AUX_META( copy_assign, void *, target,
                           const void *, source,
                *(T*)target = *(const T*)source,
                "Type is not copy assignable"
            );
    AUX_META( move_assign, void *, target,
                           void *, source,
                *(T*)target = std::move(*(T*)source),
                "Type is not move assignable"
            );
#undef AUX_META
    template< typename T, bool >
    struct destroy {
        static void f( void * target ) {
            ((T*)target)->~T();
        }
    };
    template< typename T >
    struct destroy< T, false > {
        static void f( void * ) {
            throw std::logic_error( "Type is not destructible" );
        }
    };

    template< typename T >
    struct pseudo_virtual_table : abstract_pseudo_virtual_table {
        constexpr pseudo_virtual_table() = default;
#define AUX_META( function, args, call, trait )                       \
        void function args const {                                    \
            either_helper::function<T, std::trait<T>::value>::f call; \
        }
        AUX_META( copy_construct, (void * target, const void * source),
                    (target, source), is_copy_constructible )
        AUX_META( move_construct, (void * target, void * source),
                    (target, source), is_move_constructible )
        AUX_META( copy_assign, (void * target, const void * source),
                    (target, source), is_copy_assignable )
        AUX_META( move_assign, (void * target, void * source),
                    (target, source), is_move_assignable )
        AUX_META( destroy, (void * target), (target), is_destructible )
#undef AUX_META
    };

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
