
#include <stdint.h>
#include <memory.h>

#ifdef _MSC_VER
#include <intrin.h>
#endif

#define E(N) (uint32_t(1e##N))
static const uint32_t s_pow10s[4] = { E(8), E(6), E(4), E(2) };

static const uint16_t s_100s[] = {
	'00', '10', '20', '30', '40', '50', '60', '70', '80', '90',
	'01', '11', '21', '31', '41', '51', '61', '71', '81', '91',
	'02', '12', '22', '32', '42', '52', '62', '72', '82', '92',
	'03', '13', '23', '33', '43', '53', '63', '73', '83', '93',
	'04', '14', '24', '34', '44', '54', '64', '74', '84', '94',
	'05', '15', '25', '35', '45', '55', '65', '75', '85', '95',
	'06', '16', '26', '36', '46', '56', '66', '76', '86', '96',
	'07', '17', '27', '37', '47', '57', '67', '77', '87', '97',
	'08', '18', '28', '38', '48', '58', '68', '78', '88', '98',
	'09', '19', '29', '39', '49', '59', '69', '79', '89', '99',
};
#define WW(N, I) { memcpy(b+(N), s_100s+(I), sizeof(uint16_t)); }
#define W(N, I) { b[N] = char(I) + '0'; }
#define D(N) { W(N, t >> 32); t = uint64_t(100) * uint32_t(t); }
#define DD(N) { WW(N, t >> 32); t = uint64_t(100) * uint32_t(t); }

char* u32toa_rei(uint32_t u, char* b)
{
	uint32_t floor_lg2_u;
	uint32_t lgu;
	uint64_t t;

#ifdef _MSC_VER
	unsigned long tmp;
	static_assert(sizeof(floor_lg2_u) == sizeof(tmp), "");
	_BitScanReverse(&tmp, u | 1);
	floor_lg2_u = tmp;
#else
	floor_lg2_u = 31 - __builtin_clz(u | 1);
#endif
	lgu = ((floor_lg2_u + 1) * 77) >> 9;	// approximate log_10 u / 2
	t = (uint64_t(u) * 2882303761) >> 26;	// t = u / 10^8 in 7.32 fixed point
	t += 40;								// round up low bits

	if (lgu != 4)
		t = uint32_t(t) * uint64_t(s_pow10s[lgu]);

	// correct for off-by-one error in lgu / render 1st digit in odd size values
	uint32_t first = (t >> 32);
	if (first < 10)
	{
		if (first || !u)
		{
			W(0, first);
			b++;
		}

		t = uint64_t(100) * uint32_t(t);
		lgu--;
	}

	b += (lgu << 1) + 2;
	switch (lgu) {
	case 4: DD(-10);
	case 3: DD(-8);
	case 2: DD(-6);
	case 1: DD(-4);
	case 0: DD(-2);
		// case -1:;
	}

	*b = 0;

	return b;
}

/*
char* i32toa_rei( int32_t i, char* b )
{
uint32_t u = i;
if ( i < 0 ) { *b++ = '-'; u = 0 - uint32_t( i ); }
return u32toa_rei( u, b );
}
*/
