/*
 * 如果是多核心CPU ，必须要锁内存总线，否则可能会被多核CPU访问到就不能原子操作了
 *
 */
#if (SPY_SMP)
#define SPY_SMP_LOCK  "lock;"
#else
#define SPY_SMP_LOCK
#endif

/* call &lock, 0, 1
 * if (寄存器中的值 == set ) 有锁定， ZF = 1 所以sete 返回 0
 *
 * * 沒有任何鎖定，thread 1 來拿鎖定
 * 寄存器 和 set 比較，發現 不等 ZF = 0，寄存器 = SET (value = 1)
 * thread 2來拿鎖 這時候 set = 寄存器  ZF = 1，SET = OLD （ value = 0)
 * sete %0
 * Sets the byte in the operand to 1 if the Zero Flag is set, otherwise sets
 * the operand to 0.
 * 所以 如果 锁定了，那么就res  = 1
 * 修饰寄存器 "cc" 表示状态位有变化（ZF），memory 指示 不优化指令（ORDER）
 */
static spy_inline spy_atomic_uint_t
spy_atomic_cmp_set(spy_atomic_t *lock, spy_atomic_uint_t old,
		spy_atomic_uint_t set) {
	u_char res;

	__asm__ volatile (

			SPY_SMP_LOCK
			"    cmpxchgl  %3, %1;   "
			"    sete      %0;       "

			: "=a" (res) : "m" (*lock), "a" (old), "r" (set) : "cc", "memory");

	return res;
}
