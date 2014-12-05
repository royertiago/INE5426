/* metaoperators.hpp
 * Class that automatizes the definition of relational operators
 * in classes.
 *
 * The operators generated mimic the ones in std::tuple. */
#ifndef METAOPERATORS_HPP
#define METAOPERATORS_HPP

#include <functional>

namespace mp {
    /* Each function compares the members for equality/inequality/less/etc,
     * according to the name of the function.
     * The comparisons are done lexicographically.
     *
     * The names were chosen to mimic the names of the comparison operators
     * functional objects in namespace std, header <functional>. */
    template< typename T, typename ... Members >
    bool equal_to     ( const T& rhs, const T& lhs, Members T::* ... members );
    template< typename T, typename ... Members >
    bool not_equal_to ( const T& rhs, const T& lhs, Members T::* ... members );
    template< typename T, typename ... Members >
    bool greater      ( const T& rhs, const T& lhs, Members T::* ... members );
    template< typename T, typename ... Members >
    bool less         ( const T& rhs, const T& lhs, Members T::* ... members );
    template< typename T, typename ... Members >
    bool greater_equal( const T& rhs, const T& lhs, Members T::* ... members );
    template< typename T, typename ... Members >
    bool less_equal   ( const T& rhs, const T& lhs, Members T::* ... members );

    /* These two functions together take a template class to a set of functors,
     * tho objetcs and a list of pointer to members. The members of the objecs
     * are tested for equality sequencially; when the first non equal member
     * is found, the function returns the value of applying
     * Function<Type>()( member1, member2 ). If all the members (but the last)
     * are equal, the function returs the result of applying the function to
     * the last member, no matter wheter they are equal or not.
     *
     * The main purpose of this template function is to simplify
     * the writing of the other functions in this file. */
    template< template <typename MemType> class Function,
              typename T,
              typename Member,
              typename ... Members
            >
    bool recursively_compare( const T& rhs, const T& lhs,
            Member T::* member, Members T::* ... members );

    template< template <typename MemType> class Function,
              typename T,
              typename Member
            >
    bool recursively_compare( const T& rhs, const T& lhs, Member T::* member );


    // Implementation
    template< typename T, typename ... Members >
    bool equal_to ( const T& rhs, const T& lhs, Members T::* ... members ) {
        return recursively_compare<std::equal_to>(rhs, lhs, members...);
    }

    template< typename T, typename ... Members >
    bool not_equal_to ( const T& rhs, const T& lhs, Members T::* ... members ) {
        return recursively_compare<std::not_equal_to>(rhs, lhs, members...);
    }

    template< typename T, typename ... Members >
    bool greater ( const T& rhs, const T& lhs, Members T::* ... members ) {
        return recursively_compare<std::greater>(rhs, lhs, members...);
    }

    template< typename T, typename ... Members >
    bool less ( const T& rhs, const T& lhs, Members T::* ... members ) {
        return recursively_compare<std::less>(rhs, lhs, members...);
    }

    template< typename T, typename ... Members >
    bool greater_equal ( const T& rhs, const T& lhs, Members T::* ... members ) {
        return recursively_compare<std::greater_equal>(rhs, lhs, members...);
    }

    template< typename T, typename ... Members >
    bool less_equal ( const T& rhs, const T& lhs, Members T::* ... members ) {
        return recursively_compare<std::less_equal>(rhs, lhs, members...);
    }

    template< template <typename MemType> class Function,
              typename T,
              typename Member,
              typename ... Members
            >
    bool recursively_compare( const T& rhs, const T& lhs,
            Member T::* member, Members T::* ... members )
    {
        if( rhs.*member == lhs.*member )
            return recursively_compare<Function>( rhs, lhs, members... );
        return Function<Member>()( rhs.*member, lhs.*member );
    }

    template< template <typename MemType> class Function,
              typename T,
              typename Member
            >
    bool recursively_compare( const T& rhs, const T& lhs, Member T::* member )
    {
        return Function<Member>()( rhs.*member, lhs.*member );
    }

}

#endif // METAOPERATORS_HPP
