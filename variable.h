/* variable.h
 * Structure used to pass data around the program.
 *
 * A variable can be either an integer value, or a pair of variables
 * (they are recursive). The actual definition is as a struct whose
 * first member is a pointer, and whose second member is a union
 * between a pointer and a integer. If the first pointer is null,
 * then the active member of the union is the integer, otherwise
 * is the pointer that is the active member.
 */
#ifndef VARIABLE_H
#define VARIABLE_H

#include <memory>
#include <utility> // std::move

struct Variable {
    std::unique_ptr<Variable> first;
    union {
        std::unique_ptr<Variable> second; // active if first != nullptr
        long long value; // active is first == nullptr
    };

    Variable( std::unique_ptr<Variable>&& f, std::unique_ptr<Variable>&& s ) :
        first( std::move(f) ),
        second( std::move(s) )
    {}

    explicit Variable( long long value ) :
        first( nullptr ),
        value( value )
    {}

    ~Variable() {
        if( first ) second.reset();
    }

    /* Due to the complexity added by the union, it is easier
     * to simply disallow copies, moves and assignments. */
    Variable( const Variable & ) = delete;
    Variable( Variable && ) = delete;
    Variable & operator=( const Variable & ) = delete;
    Variable & operator=( Variable && ) = delete;

    std::unique_ptr<Variable> clone() const {
        if( !first ) return std::make_unique<Variable>( value );
        return std::make_unique<Variable>(
                std::move( first->clone() ),
                std::move( second->clone() )
            );
    }
};

#endif // VARIABLE_H
