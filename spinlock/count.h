#include <stdio.h>


#define PER_CPU_ATTRIBUTES
#define PER_CPU_BASE_SECTION ".data"
# define __percpu	__attribute__((noderef, address_space(3)))
#define __PCPU_ATTRS(sec)						\
	__percpu __attribute__((section(PER_CPU_BASE_SECTION sec)))	\
	PER_CPU_ATTRIBUTES

#define DEFINE_PER_CPU_SECTION(type, name, sec)				\
	__PCPU_ATTRS(sec) __typeof__(type) name

#define DEFINE_PER_CPU(type, name)					\
	DEFINE_PER_CPU_SECTION(type, name, "")

/*-------------------------------------------*/
#define INIT_PREEMPT_COUNT	PREEMPT_OFFSET
#define PREEMPT_OFFSET	(1UL << PREEMPT_SHIFT)
#define PREEMPT_SHIFT	0


DEFINE_PER_CPU(int, __preempt_count) = INIT_PREEMPT_COUNT;

