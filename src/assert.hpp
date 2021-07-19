#ifndef RNES_ASSERT_INCLUDED
#define RNES_ASSERT_INCLUDED

#include <iostream>
#include <cstdlib>

#define ASSERT(x, msg) do {\
    if (!(x)) {\
        std::cerr << "Assertion failed; " << __FILE__ << " (" << __func__ << " : " << __LINE__ << ")\n";\
        std::exit(EXIT_FAILURE);\
    }\
} while(false)

#endif
