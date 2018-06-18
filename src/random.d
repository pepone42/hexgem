import core.stdc.stdint;
import random;

static uint32_t[16] state;
static uint32_t index;
 
void init_rand(uint32_t x)
{
	int i;
	index=0;
	state[0]=x^0xf68a9fc1;
	for (i=1; i<16; i++) {
		state[i] = (0x6c078965U * (state[i-1] ^ (state[i-1] >> 30)) + i); 
	}

}
 
uint32_t random_well512()
{
	uint a = state[index];
	uint c = state[(index+13)&15];
	uint b = a^c^(a<<16)^(c<<15);
	c = state[(index+9)&15];
	c ^= (c>>11);
	a = state[index] = b^c;
	uint d = a^((a<<5)&0xda442d20U);
	index = (index + 15)&15;
	a = state[index];
	state[index] = a^b^d^(a<<2)^(b<<18)^(c<<28);
	return state[index];
}


uint32_t random_max(uint32_t max) {
	uint32_t base_random = random_well512(); /* in [0, RAND_MAX] */
	uint32_t remainder,bucket,range;
	if (RNDMAX == base_random) return random_max( max);
	/* now guaranteed to be in [0, RAND_MAX) */
	//int range       = max - min,
	remainder   = RNDMAX % max,
	bucket      = RNDMAX / max;
	/* There are range buckets, plus one smaller interval
	 within remainder of RAND_MAX */
	if (base_random < RNDMAX - remainder) {
		return base_random/bucket;
	} else {
		return random_max (max);
	}
}