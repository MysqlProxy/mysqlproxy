#include <spy_core.h>
#include <spy_config.h>

u_char * spy_cdecl
spy_slprintf(u_char *buf, u_char *last, const char *fmt, ...) {
	u_char *p;
	va_list args;

	va_start(args, fmt);
	p = spy_vslprintf(buf, last, fmt, args);
	va_end(args);

	return p;
}

u_char *
spy_vslprintf(u_char *buf, u_char *last, const char *fmt, va_list args) {

	const char *r_fmt;
	u_char *p, tempn[SPY_INT64_LEN + 1], tempf[SPY_DOUBLE_FRAC_LEN + 1],
			tempr[SPY_INT64_LEN + SPY_DOUBLE_FRAC_LEN + 2], *pr;
	int c, i; // char 临时字符
	u_char zero; // 填充字符
	int64_t i64;
	uint64_t ui64;
	size_t len;
	double d, frac;

	//size_t slen; // 传参进入的解码长度
	spy_flag_t align; // 对齐方式默认 0 : 右对齐, 1 : 左对齐
	spy_uint_t frac_width, width, hex, l_int, s_int;
	// 小数字点，长整型int，短整形int，长双精度double

	while (*fmt && buf < last) {
		if (*fmt != '%') {
			*buf++ = *fmt++; // 非解析字符继续偏移
		} else {
			// %[flags][width][.precision][length]specifier

			width = 0;
			align = 0;
			frac_width = 0;
			l_int = 0;
			s_int = 0;
			r_fmt = fmt;
			len = 0;
			zero = ' ';
			hex = 0;

			// slen = (size_t) -1;
			fmt++;
			for (;;) {
				switch (*fmt) {

				case '-': // flags
					align = 1;
					fmt++;
					continue;
				case '+':
					align = 0;
					fmt++;
					continue;
				case '#':
					fmt++;
					continue;
				case '0':
					zero = '0';
				default:
					break;
				}
				break;
			}

			// width
			while (*fmt >= '0' && *fmt <= '9') {
				width = width * 10 + *fmt++ - '0';
			}

			for (;;) {
				switch (*fmt) {
				case '*': // width
					width = va_arg(args, size_t);
					fmt++;
					continue;
				case '.': // .precision
					while (*fmt >= '0' && *fmt <= '9') {
						frac_width = frac_width * 10 + *fmt++ - '0';
					}
					continue;
				case 'l': // length
					l_int = l_int + 1;
					fmt++;
					continue;
				case 'h':
					s_int = s_int + 1;
					fmt++;
					continue;
				case 'x':
					hex = 1;
					fmt++;
					continue;
				case 'X':
					hex = 2;
					fmt++;
					continue;

				default:
					break;
				}
				break;
			}

			// specifier
			switch (*fmt) {

			case 's':
				p = va_arg(args, u_char *);

				while (*p) {
					p++;
					len++;
				}

				buf = spy_printf_pad(buf, last, width, len, zero, p - len,
						align);
				fmt++;

				continue;

			case 'c':
				c = (u_char) (((u_char) va_arg(args, int)) & 0xff);
				buf = spy_printf_pad(buf, last, width, 1, zero, (u_char *) &c,
						align);
				fmt++;

				continue;

			case 'd':
			case 'i':
				if ((SPY_LONG_SIZE == 8 && l_int == 1) || l_int == 2) {
					i64 = (int64_t) va_arg (args, int64_t);
				} else {
					i64 = (int64_t) va_arg(args, int32_t);
				}

				break;
			case 'u':
				if ((SPY_LONG_SIZE == 8 && l_int == 1) || l_int == 2) {
					ui64 = (uint64_t) va_arg (args, uint64_t);
				} else {
					ui64 = (uint64_t) va_arg (args, uint32_t);
				}
				break;
			case 'p':
				break;

			case 'f':
				d = (double) va_arg (args, double);
				frac_width = (frac_width == 0 ? (l_int > 0 ? 15 : 6)
						: frac_width);
				i = 1;
				pr = tempr;
				tempf[0] = '.';

				if (d < 0) {
					*buf++ = '-';
					d = -d;
				}

				frac = spy_modf(d, &ui64);
				p = tempn + SPY_INT64_LEN;
				p = spy_sprintf_num(ui64, zero, align, 0, width, p);

				do {
					frac *= 10;
					frac = spy_modf(frac, &ui64);
					tempf[i] = ((u_char) ui64) + '0';
					i++;
				} while (i <= frac_width && frac != 0);

				len = (tempn + SPY_INT64_LEN) - p + i;

				pr = spy_cpymem(pr, p, (tempn + SPY_INT64_LEN) - p);
				spy_memcpy(pr, tempf, i);

				buf = spy_printf_pad(buf, last, width, len, zero, tempr, align);

				fmt++;
				continue;

			case 'Z':
				*buf++ = '\0';
				fmt++;

				continue;

			case 'N':
				*buf++ = LF;
				fmt++;

			case '%':
				*buf++ = '%';
				fmt++;

				continue;

			default:
				fmt++;
				buf = spy_cpymem(buf, r_fmt, fmt - r_fmt);
				continue;
			}

			if (i64 < 0) {
				*buf++ = '-';
				ui64 = (uint64_t) - i64;

			} else {
				ui64 = (uint64_t) i64;
			}

			p = tempn + SPY_INT64_LEN;
			p = spy_sprintf_num(ui64, zero, align, hex, width, p);
			buf = spy_printf_pad(buf, last, width, (tempn + SPY_INT64_LEN) - p,
					zero, p, align);
			fmt++;

		} // END ELSE
	} // END WHILE

	return buf;
} // END FUNCTION

u_char *
spy_printf_pad(u_char *buf, u_char *last, spy_uint_t width, size_t len,
		u_char zero, u_char *p, spy_uint_t align) {

	int i;
	u_char *t;
	width = spy_ldiff(width, len);

	len = spy_min(len,(size_t) (last - buf));
	t = buf;
	if (align == 0) {
		if (width > 0) {
			buf = buf + len;
		}
	} else if (align == 1) {
		if (width > 0) {
			t = buf + width;
		}
	}

	for (i = 0; i < width; i++) {
		*buf++ = zero;
	}

	spy_memcpy(t, p, len);

	return t + len + width;
}

u_char *
spy_sprintf_num(uint64_t ui64, u_char zero, spy_uint_t align, spy_uint_t hex,
		spy_uint_t width, u_char *pnum) {

	uint32_t ui32;
	static u_char hexstr[] = "0123456789abcdef";
	static u_char HEXstr[] = "0123456789ABCDEF";

	if (hex == 0) {
		if (ui64 <= SPY_MAX_UINT32_VALUE) {
			ui32 = (uint32_t) ui64;
			do {
				*--pnum = (u_char) (ui32 % 10 + '0');
			} while (ui32 /= 10);
		} else {
			do {
				*--pnum = (u_char) (ui64 % 10 + '0');
			} while (ui64 /= 10);
		}
	} else if (hex == 1) {
		do {
			*--pnum = hexstr[(uint32_t)(ui64 & 0xf)];

		} while (ui64 >>= 4);
	} else if (hex == 2) {
		do {
			*--pnum = HEXstr[(uint32_t)(ui64 & 0xf)];

		} while (ui64 >>= 4);
	}

	return pnum;
	//len = (size_t) spy_min((last - buf), (temp + SPY_INT64_LEN) - p);
}

#ifdef _SPY_STRING_UNIT_TEST_

int main() {
#if 0

	u_char buf[256];
	u_char *p;

	p = buf;
	spy_printf_pad(p, 'a', 10);
	buf[255] = '\0';
	printf("%s\n", buf);
#endif

	u_char buf[10];
	u_char *p, *last;
	last = buf + 10;
	p = buf;
	p = spy_slprintf(p, last, "%-lf\n", 12123123123.5123123);
	write(1, buf, p - buf);
	//write(1, c, 1);
	// 1 11111000 000000000000000000000

	exit(EXIT_SUCCESS);
}

#endif
