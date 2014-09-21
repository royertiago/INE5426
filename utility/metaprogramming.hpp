/* metaprogramming.hpp
 * Utilities for metaprograms.
 */
#ifndef METAPROGRAMMING_HPP
#define METAPROGRAMMING_HPP

namespace mp {
    /* Value metafunction that returns the logical and of its inputs. */
    template< bool ... > struct logical_and;

    /* Value metafuction that returns the logical or of its inputs. */
    template< bool ... > struct logical_or;

    /* Value metafunction that returns the index of type T in list List.
     * If T is not in list, returns -1.
     * If T is multiple times, returns first instance.
     * Return type is unsigned, List is indexed in zero. */
    template< typename T, typename ... List > struct type_index;



    // Implementation
    template<> struct logical_and<> {
        static constexpr bool value = true;
    };
    template< bool Head, bool ... Tail >
    struct logical_and< Head, Tail... > {
        static constexpr bool value = Head && logical_and< Tail... >::value;
    };

    template<> struct logical_or<> {
        static constexpr bool value = false;
    };
    template< bool Head, bool ... Tail >
    struct logical_or< Head, Tail... > {
        static constexpr bool value = Head || logical_or< Tail... >::value;
    };


    template< typename T > struct type_index<T> {
        static constexpr unsigned value = -1;
    };
    template< typename T, typename ... List >
    struct type_index<T, T, List...> {
        static constexpr unsigned value = 0;
    };
    template< typename T, typename Head, typename ... Tail >
    struct type_index<T, Head, Tail...> {
        static constexpr unsigned value =
            type_index<T, Tail...>::value == -1u ?
            -1 :
            type_index<T, Tail...>::value + 1;
    };
            
}

#endif // METAPROGRAMMING_HPP
