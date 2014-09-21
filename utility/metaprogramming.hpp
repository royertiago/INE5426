/* metaprogramming.hpp
 * Utilities for metaprograms.
 */
#ifndef METAPROGRAMMING_HPP
#define METAPROGRAMMING_HPP

namespace mp {
    /* Value metafunction that returns the logical and of its inputs. */
    template< bool ... > struct logical_and;

    /* Value metafuction that returs the logical or of its inputs. */
    template< bool ... > struct logical_or;


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

}

#endif // METAPROGRAMMING_HPP
