#include <random>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#pragma once

#define PI M_PI
#define TWO_PI (2.0 * M_PI)

// Returns a double (0 <= x < mod)
double wyrandom_d(int mod)
{
#ifdef __EMSCRIPTEN__
    return (double)(emscripten_random() * mod);
#else
    return (double)(rand() % (mod * 100)) / 100.0;
#endif
}

// Returns an integer (0 <= x < mod)
template <class T = Uint16>
T wyrandom(int mod)
{
#ifdef __EMSCRIPTEN__
    // Javascript returns a value from 0 to 1.
    return (T)(emscripten_random() * mod);
#else
    // http://www.cplusplus.com/reference/cstdlib/RAND_MAX/
    // A number between 0 to RAND_MAX. Guarantees at least 32767.
    return (T)(rand() % mod);
#endif
}
