#include <random>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

// Returns a Uint16 number from 0 to mod.
Uint16 random(int mod)
{
#ifdef __EMSCRIPTEN__
    // Javascript returns a value from 0 to 1.
    return emscripten_random() * mod;
#else
    // http://www.cplusplus.com/reference/cstdlib/RAND_MAX/
    // A number between 0 to RAND_MAX. Guarantees at least 32767.
    return rand() % mod;
#endif
}