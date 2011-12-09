#include <spy_core.h>
#include <spy_config.h>

double spy_modf(double calc, uint64_t *intp) {

	double frac;
	uint64_t *d;
	//int p = 51;
	uint64_t e, z;

	calc = spy_abs(calc);
	if (calc < 1) {
		*intp = 0;
		return calc;
	}

	d = (uint64_t *) &calc;
	e = ((*d & 0x7FF0000000000000) >> 52) - 1023;
	z = (*d & 0xFFFFFFFFFFFFF) | 0x10000000000000;

	*intp = z >> (52 - e);
	frac = calc - *intp;
	return frac;
}

#ifdef _SPY_MATH_UNIT_TEST_

int main() {
	printf("%d", (0.000f == 0));
	double t = 10;
	uint64_t intp;
	double frac = (double) spy_modf(t, &intp);
	printf("intp : %llu frac : %lf\n", intp, frac);

	exit(EXIT_SUCCESS);
}

#endif
