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
#include <string>
#include <unordered_map>
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

bool operator==( const Variable& lhs, const Variable& rhs ) {
    if( lhs.first )
        return rhs.first &&
                *lhs.first == *rhs.first &&
                *lhs.second == *rhs.second;
    return !rhs.first &&
        lhs.value == rhs.value;
}

/* Class that mantains a list of variables and its names.
 * It is used in variable decomposition, done during overload selection. */
class VariableTable {
    std::unordered_map< std::string, std::unique_ptr<Variable> > table;

public:
    /* Inserts a variable in the table.
     * If the variable exists in the table and the stored value is the
     * same, this method silently ignores the insertion.
     * Otherwise, semantic_error is thrown. */
    void insert( std::string name, std::unique_ptr<Variable>&& variable );

    /* Returns a clone of the specified variable. */
    std::unique_ptr<Variable> retrieve( std::string name ) const;
};
#endif // VARIABLE_H
