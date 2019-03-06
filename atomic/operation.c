#include <stdint.h>
#include <stdio.h>
typedef unsigned long u64;
typedef enum {false, true} bool;

#define notrace __attribute__((no_instrument_function))

#define typecheck(type,x) \
({	type __dummy; \
	typeof(x) __dummy2; \
	(void)(&__dummy == &__dummy2); \
	1; \
})
extern inline unsigned long native_save_fl(void)
{
	unsigned long flags;

	/*
	 * "=rm" is safe here, because "pop" adjusts the stack before
	 * it evaluates its effective address -- this is part of the
	 * documented behavior of the "pop" instruction.
	 */
	asm volatile("# __raw_save_flags\n\t"
		     "pushf ; pop %0"
		     : "=rm" (flags)
		     : /* no input */
		     : "memory");

	return flags;
}

extern inline void native_restore_fl(unsigned long flags)
{
	asm volatile("push %0 ; popf"
		     : /* no output */
		     :"g" (flags)
		     :"memory", "cc");
}

static inline notrace unsigned long arch_local_save_flags(void)
{
	return native_save_fl();
}

static inline void native_irq_disable(void)
{
	asm volatile("cli": : :"memory");
}


static inline notrace void arch_local_irq_disable(void)
{
	native_irq_disable();
}
static inline notrace unsigned long arch_local_irq_save(void)
{
	unsigned long flags = arch_local_save_flags();
	//cli 명령 interupt signal 무시
    //arch_local_irq_disable();
	return flags;
}

#define raw_local_irq_save(flags)			\
	do {						\
		typecheck(unsigned long, flags);	\
		flags = arch_local_irq_save();		\
	} while (0)

#define local_irq_save(flags)					\
	do {							\
		raw_local_irq_save(flags);			\
	} while (0)
    
#define IPL_MAX		7
static inline bool arch_irqs_disabled_flags(unsigned long flags)
{
	return flags == IPL_MAX;
}
#define raw_irqs_disabled_flags(flags)			\
	({						\
		typecheck(unsigned long, flags);	\
		arch_irqs_disabled_flags(flags);	\
	})

static inline notrace void arch_local_irq_restore(unsigned long flags)
{
	native_restore_fl(flags);
}
#define raw_local_irq_restore(flags)			\
	do {						\
		typecheck(unsigned long, flags);	\
		arch_local_irq_restore(flags);		\
	} while (0)
# define trace_hardirqs_on()		do { } while (0)
# define trace_hardirqs_off()		do { } while (0)

#define local_irq_restore(flags)			\
	do {						\
		if (raw_irqs_disabled_flags(flags)) {	\
			raw_local_irq_restore(flags);	\
			trace_hardirqs_off();		\
		} else {				\
			trace_hardirqs_on();		\
			raw_local_irq_restore(flags);	\
		}					\
	} while (0)

//software atomic operation
static inline
unsigned long __xchg(unsigned long x, volatile void *ptr, int size)
{
	unsigned long ret, flags;
	local_irq_save(flags);
    printf("local_irq_save(flags) : %lx \n", flags);
	ret = *(volatile u64 *)ptr;
	*(volatile u64 *)ptr = x;
    printf("local_irq_save(ptr) : %p \n", ptr);
	local_irq_restore(flags);
	return ret;
}



#define xchg(ptr, x) ({							\
    ((__typeof__(*(ptr)))						\
    __xchg((unsigned long)(x), (ptr), sizeof(*(ptr))));	\
})


#define atomic_xchg(v, new)      (xchg(&((v)->counter), new))

int main()
{
    int a = 1;
    int b = 2;
    printf("%d %d\n", a, b);
    xchg(&a, b);
    printf("%d %d\n", a, b);

    return 0;
}
