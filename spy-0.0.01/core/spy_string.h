

#ifndef _SPY_STRING_H_INCLUDED_
#define _SPY_STRING_H_INCLUDED_

#include <spy_core.h>
#include <spy_config.h>

typedef struct {
    size_t      len; // 字符串长度
    u_char     *data; // 字符串
} spy_str_t;


/*
 * In : buf 字符串缓存,last 最大的传入字符串位置,fmt 字符串解析格式,args 可以参数
 * Out : 转义后的字符串
 * Desc : 主要实现字符串格式解析 类似于 printf
 * Etc :
 * Ref : http://www.cplusplus.com/reference/clibrary/cstdio/printf/
 */

u_char *
spy_vslprintf(u_char *buf, u_char *last, const char *fmt, va_list args);

#define spy_memcpy(dst, src, n)   (void) memcpy(dst, src, n)
#define spy_cpymem(dst, src, n)   (((u_char *) memcpy(dst, src, n)) + (n))

#endif

