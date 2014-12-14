/* variable.h
 * Structure used to pass data around the program.
 *
 * A variable can be either an integer value, or a pair of variables
 * (they are recursive). The actual definition is as a struct whose
 * first member is a pointer, and whose second member is a union
 * between a pointer and a integer. If the first pointer is null,
 * then the active member of the union is the integer, otherwise
 * is the pointer that is the active member.
 *
 * Since the active member is set at construction, this information
 * should not change in the execution of the program.
 */
#ifndef VARIABLE_H
#define VARIABLE_H

#include <memory>
#include <string>
#include <unordered_map>

struct Variable {
    std::unique_ptr<Variable> first;
    union {
        std::unique_ptr<Variable> second; // active if first != nullptr
        long long value; // active is first == nullptr
    };

    /* Move-constructs first and second. */
    Variable( std::unique_ptr<Variable>&& f, std::unique_ptr<Variable>&& s );

    /* Set first = nullptr and initializes value. */
    explicit Variable( long long value );

    /* The destructor is not the default because we have a union
     * and we must call the destructor of second conditionally. */
    ~Variable();

    /* Due to the complexity added by the union, it is easier
     * to simply disallow copies, moves and assignments. */
    Variable( const Variable & ) = delete;
    Variable( Variable && ) = delete;
    Variable & operator=( const Variable & ) = delete;
    Variable & operator=( Variable && ) = delete;

    /* Returns a deep copy of this object. */
    std::unique_ptr<Variable> clone() const;
};

/* Returns true if the objects are of the same type
 * and share the same value, false otherwise. */
bool operator==( const Variable& lhs, const Variable& rhs );

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
