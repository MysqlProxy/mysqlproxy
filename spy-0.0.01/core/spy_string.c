#include <spy_core.h>
#include <spy_config.h>

u_char *
spy_repl_char(u_char *dst, u_char src, spy_uint_t num) {
	int i;
	for (i = 0; i < num; i++) {
		*dst++ = src;
	}
	return dst;
}

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
	u_char *p; // buf 字符
	int c; // char 临时字符
	u_char zero; // 填充字符
	int64_t i64;
	uint64_t ui64;
	int32_t i32;
	uint32_t ui32;
	double d;
	//size_t slen; // 传参进入的解码长度
	spy_flag_t align; // 对齐方式默认 0 : 右对齐, 1 : 左对齐
	spy_uint_t frac_width, width, hex, l_int, s_int, l_double, len;
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
			l_double = 0;
			r_fmt = fmt;
			len = 0;
			zero = ' ';

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
					break;
				case 'h':
					s_int = s_int + 1;
					fmt++;
					break;
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

				if (align == 0) {
					while (*p && buf < last) {
						*buf++ = *p++;
						len++;
					}
					buf = spy_repl_char(buf, zero,
							spy_min(spy_ldiff(width, len), last - buf));
				} else if (align == 1) {
					while (*p) {
						p++;
						len++;
					}
					len = spy_min(len, last - buf);
					buf = spy_repl_char(buf, zero, spy_ldiff(width, len));
					buf = spy_cpymem(buf, p - len, len);
				}
				fmt++;

				continue;

			case 'c':
				c = va_arg(args, int);
				*buf++ = (u_char) (c & 0xff);
				fmt++;

				continue;

			case 'd':
			case 'i':
				if ((SPY_LONG_SIZE == 8 && l_int == 1) || l_int == 2) {
					i64 = (int64_t) va_arg (args, int64_t);
				} else {
					i32 = (int32_t) va_arg(args, int32_t);
				}

				break;
			case 'u':
				if ((SPY_LONG_SIZE == 8 && l_int == 1) || l_int == 2) {
					ui64 = (uint64_t) va_arg (args, uint64_t);
				} else {
					ui32 = (uint32_t) va_arg (args, uint32_t);
				}
				break;
			case 'p':
				break;

			case 'f':
				d = (double) va_arg (args, double);
				if (d < 0) {
					*buf++ = '-';
					d = -d;
				}
				fmt++;
				continue;

			case 'Z':
				*buf++ = '\0';
				fmt++;

				continue;

			case 'N':
#if (SPY_WIN32)
				*buf++ = CR;
#endif
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

		} // END ELSE
	} // END WHILE

	return buf;
} // END FUNCTION

#ifdef _SPY_STRING_UNIT_TEST_

int main() {
#if 0
	u_char buf[256];
	u_char *p;

	p = buf;
	spy_repl_char(p, 'a', 10);
	buf[255] = '\0';
	printf("%s\n", buf);

#endif
	u_char buf[256];
	u_char *p, *last;
	last = buf + 256;
	p = buf;
	int i = 20; //12345
	p = spy_slprintf(p, last, "%+0*s%%%c\n", i, "Hello World", 'a');
	write(1, buf, p - buf);

	exit(EXIT_SUCCESS);
}

#endif
