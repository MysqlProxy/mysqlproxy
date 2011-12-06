#include <spy_core.h>
#include <spy_config.h>

double spy_modf(double calc, uint64_t *intp) {

	double frac;
	uint64_t * d;
	int p = 51;
	uint64_t e, z;

	calc = spy_abs(calc);
	if (calc < 1) {
		*intp = 0;
		return calc;
	}

	d = (uint64_t *) &calc;
	e = ((*d & 0x7FF0000000000000) >> 52) - 1023;
	// printf("e : %llu\n", e);
	z = *d & 0xFFFFFFFFFFFFF;

	printf("z : %llu\n", z);
	while (((z & ((uint64_t)1 << p)) !=  ((uint64_t)1 << p)) && p) {
		e++;
		p--;
	}

	*intp = z >> (51 - e);
	frac = calc - *intp;
	return frac;
}

#ifdef _SPY_MATH_UNIT_TEST_

int main() {
	double t = 120.5;
	uint64_t intp;
	double frac = spy_modf(t, &intp);
	printf("intp : %llu frac : %lf\n", intp, frac);

	exit(EXIT_SUCCESS);
}

#endif
