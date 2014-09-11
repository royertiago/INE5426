/* position_iterator.h
 * Itearot wrapper that does line and column counting,
 * based on the character '\n'.
 */
#include <cstddef>
#include <iterator>
#include <type_traits>

template< typename Iterator >
class position_iterator {
    Iterator _iterator;
    std::size_t _line, _column;

public:
    /* Constructs an position iterator based on the
     * specified iterator. */
    position_iterator( Iterator iterator ) :
        _iterator( iterator ),
        _line( 1 ),
        _column( 1 )
    {}

    /* Constructs a null position iterator. */
    position_iterator() :
        position_iterator( Iterator() )
    {}

    /* Basic operations: dereference, indirection, increment. */
    auto operator*()        { return *_iterator; }
    auto operator*() const  { return *_iterator; }
    auto operator->()       { return _iterator.operator->(); }
    auto operator->() const { return _iterator.operator->(); }
    auto operator++(int)    { auto tmp = *this; ++*this; return tmp; }

    position_iterator& operator++() {
        if( *_iterator == '\n' ) {
            ++_line;
            _column = 1;
        } 
        else
            ++_column;

        ++_iterator;
        return *this;
    }

    /* Relational operators: == != < > <= >= 
     * Defined only if the respective iterator
     * defines. */
#define POSITION_ITERATOR_FORWARD_OPERATOR(op)                  \
    friend bool operator op ( const position_iterator& lhs,     \
                              const position_iterator& rhs ) {  \
        return lhs._iterator op rhs._iterator;                  \
    }
    POSITION_ITERATOR_FORWARD_OPERATOR(==)
    POSITION_ITERATOR_FORWARD_OPERATOR(!=)
    POSITION_ITERATOR_FORWARD_OPERATOR(<=)
    POSITION_ITERATOR_FORWARD_OPERATOR(>=)
    POSITION_ITERATOR_FORWARD_OPERATOR(< )
    POSITION_ITERATOR_FORWARD_OPERATOR(> )
#undef POSITION_ITERATOR_FORWARD_OPERATOR

    /* Aditional functionality: retrieving current line and column */
    auto line() const   { return _line; }
    auto column() const { return _column; }
};

namespace std {
    template< typename Iterator >
    struct iterator_traits< position_iterator<Iterator> > :
        iterator_traits<Iterator>
    {
        typedef typename conditional<
            is_same<
                typename iterator_traits<Iterator>::iterator_category,
                random_access_iterator_tag
            >::value,
            bidirectional_iterator_tag,
            typename iterator_traits<Iterator>::iterator_category
        >::type iterator_category;
    };
} // namespace std
