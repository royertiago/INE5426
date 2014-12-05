/* symbol.cpp
 * Implementation of symbol.h.
 */
#include "symbol.h"

static unsigned last_category_value = 0;

std::unique_ptr<Category> Category::next( std::string name ) {
    return std::make_unique<Category>( name, last_category_value++ );
}
