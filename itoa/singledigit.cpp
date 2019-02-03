#include <stdint.h>

#ifdef WIN32
#inlclude <intrin.h>
#endif

#define W(N, I) { b[N] = char(I) + '0'; }
#define D(N) { W(N, t >> 32); t = uint64_t(10) * uint32_t(t); }
#define E(N) (uint32_t(1e##N))

//#define NULL_TERMINATE

char* u32toa_rei(uint32_t u, char* b)
{
	if (!u)
	{
		b[0] = '0';
#ifdef NULL_TERMINATE
		b[1] = 0;
#endif
		return b + 1;
	}

	uint32_t floor_lg2_u;
#ifdef WIN32
	unsigned long tmp;
	static_assert(sizeof(floor_lg2_u) == sizeof(tmp), "");
	_BitScanReverse(&tmp, u);
	floor_lg2_u = tmp;
#else
	floor_lg2_u = 31 - __builtin_clz(u);
#endif
	uint32_t lgu = ((floor_lg2_u + 1) * 77) >> 8; // approximate log_10 u
	uint64_t t = (uint64_t(u) * 2305843009) >> 29;  // t = u / 10^9 in 4.32 fixed point
	t += 4;											  // round up low bits

	static const uint32_t es[] = { E(9), E(8), E(7), E(6), E(5), E(4), E(3), E(2), E(1), 0 };
	uint64_t t_mul = uint32_t(t) * uint64_t(es[lgu]);
	t = (lgu != 9) ? t_mul : t;
	if (!(t >> 32))
	{
		lgu--;
		t *= 10;
	}

	// now we should have the first digit
	// assert( (t>>32) >= 1 && (t>>32) <= 9 );

	b += lgu + 1;
	switch (lgu) {
	case 9: D(-10);
	case 8: D(-9);
	case 7: D(-8);
	case 6: D(-7);
	case 5: D(-6);
	case 4: D(-5);
	case 3: D(-4);
	case 2: D(-3);
	case 1: D(-2);
	case 0: W(-1, t >> 32);
	}

#ifdef NULL_TERMINATE
	*b = 0;
#endif

	return b;
}

char* i32toa_rei(int32_t i, char* b)
{
	uint32_t u = i;
	if (i < 0) { *b++ = '-'; u = 0 - uint32_t(i); }
	return u32toa_rei(u, b);
}
