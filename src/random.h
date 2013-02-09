#ifndef H_RANDOM
#define H_RANDOM
#include <stdint.h>

#define RNDMAX 0xFFFFFFFFU

void init_rand(uint32_t x);
uint32_t random_well512(void);
uint32_t random_max(uint32_t max);

#endif