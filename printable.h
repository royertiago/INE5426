/* printable.h
 * Abstract base class to objects that have the method print_to.
 * This header define the printing operator for all this classes.
 */
#ifndef PRINTABLE_HPP
#define PRINTABLE_HPP

#include <iosfwd>

struct Printable {
    virtual ~Printable() = default;
    virtual std::ostream& print_to( std::ostream& ) const = 0;

    /* Returs a deep copy of this object.
     * The ownterwship of the returned pointer is transferred
     * to the caller; it is its responsibility to delete afterwards
     * (or to assign to a smart pointer).
     *
     * This method returs a raw pointer instead of a smart pointer
     * to allow covariance. */
     virtual Printable * clone() const = 0;
};

inline std::ostream& operator<<( std::ostream& os, const Printable& obj ) {
    return obj.print_to( os );
}

#endif // PRINTABLE_HPP
