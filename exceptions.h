/* exceptions.h
 * List of all exceptions thrown by the program. 
 */
#include <stdexcept>

struct file_error : public std::runtime_error {
    file_error( const char * what ) :
        runtime_error( what )
    {}
    file_error( std::string & what ) :
        runtime_error( what )
    {}
};
