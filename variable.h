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

struct Variable {
    std::unique_ptr<Variable> first;
    union {
        std::unique_ptr<Variable> second; // active if first != nullptr
        long long value; // active is first == nullptr
    };
};

#endif // VARIABLE_H
