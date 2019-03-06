#include "count.h"

#define barrier() __asm__ __volatile__("": : :"memory")
#define __percpu_prefix		""
#define __percpu_arg(x)		__percpu_prefix "%" #x

#define percpu_add_op(var, val)						\
do {									\
	typedef typeof(var) pao_T__;					\
	const int pao_ID__ = (__builtin_constant_p(val) &&		\
			      ((val) == 1 || (val) == -1)) ?		\
				(int)(val) : 0;				\
	if (0) {							\
		pao_T__ pao_tmp__;					\
		pao_tmp__ = (val);					\
		(void)pao_tmp__;					\
	}								\
		if (pao_ID__ == 1)					\
			asm("incl "__percpu_arg(0) : "+m" (var));	\
		else if (pao_ID__ == -1)				\
			asm("decl "__percpu_arg(0) : "+m" (var));	\
		else							\
			asm("addl %1, "__percpu_arg(0)			\
			    : "+m" (var)				\
			    : "ri" ((pao_T__)(val)));			\
}while(0)

#define raw_cpu_add_4(pcp, val)		percpu_add_op((pcp), val)
const int __temp = 1;

static __always_inline void __preempt_count_add(int val)
{
	raw_cpu_add_4(__preempt_count, val);
}

static __always_inline void __preempt_count_sub(int val)
{
	raw_cpu_add_4(__preempt_count, -val);
}

#define preempt_count_add(val)	__preempt_count_add(val)
#define preempt_count_sub(val)	__preempt_count_sub(val)

#define preempt_count_inc() preempt_count_add(1)
#define preempt_count_dec() preempt_count_sub(1)

#define preempt_disable() \
do { \
	preempt_count_inc(); \
	barrier(); \
} while (0)

#define preempt_enable() \
do { \
	barrier(); \
	preempt_count_dec(); \
} while (0)


/*
int main()

{
    preempt_disable();
    printf("%d\n",__preempt_count);
    int a = 0;
    int b = 1;
    asm("addl %1, "__percpu_arg(0)			\
			    : "+m" (a)				\
			    : "ri" (b));			\
    printf("%d\n", a);
}
*/
