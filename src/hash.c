#include "hash.h"

#define HASH_MULTIPLIER 37

uint32_t hash_multiplication(const char *s)
{
    uint32_t h = 0;
    uint8_t *c = (uint8_t *) s;

    while (*c != '\0')
        h = h * HASH_MULTIPLIER + *c++;

    return h;
}
