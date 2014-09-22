/* printable.h
 * Abstract base class to objects that have the method print_to.
 * This header define the printing operator for all this classes.
 */
#ifndef PRINTABLE_HPP
#define PRINTABLE_HPP

#include <ostream>

struct Printable {
    virtual ~Printable() = default;
    virtual std::ostream& print_to( std::ostream& ) const = 0;
};

inline std::ostream& operator<<( std::ostream& os, const Printable& obj ) {
    return obj.print_to( os );
}

#endif // PRINTABLE_HPP
