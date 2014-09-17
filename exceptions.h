/* exceptions.h
 * List of all exceptions thrown by the program. 
 */
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>

struct file_error : public std::runtime_error {
    file_error( const char * what ) :
        runtime_error( what )
    {}
    file_error( std::string & what ) :
        runtime_error( what )
    {}
};
#endif // EXCEPTIONS_H
