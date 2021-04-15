
#ifndef SDL2_HELPER
#define SDL2_HELPER

#include <SDL2\SDL.h>
#include <stdio.h>

#ifndef CheckStatu
#define CheckStatu(x)                                \
{                                                    \
    int result = x;                                  \
    if(x != 0) printf(stdout, "this is Error")       \
}                                                    

#endif // !CheckStatu




#endif // !SDL2_HELPER


