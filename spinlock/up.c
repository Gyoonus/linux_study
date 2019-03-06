#include "preemtion.h"

typedef struct raw_spinlock {
    int lock;      
}raw_spinlock_t;

typedef struct spinlock {
	union {
		struct raw_spinlock rlock;
    };
}spinlock_t;

#define __LOCK(lock) \
  do { preempt_disable(); } while (0)

#define __UNLOCK(lock) \
  do { preempt_enable();  } while (0)


#define _raw_spin_lock(lock)			__LOCK(lock)
#define raw_spin_lock(lock)	_raw_spin_lock(lock)

#define _raw_spin_unlock(lock)			__UNLOCK(lock)
#define raw_spin_unlock(lock)		_raw_spin_unlock(lock)

static __always_inline void spin_unlock(spinlock_t *lock)
{
	raw_spin_unlock(&lock->rlock);
}

static __always_inline void spin_lock(spinlock_t *lock)
{
	raw_spin_lock(&lock->rlock);
}

int main()
{
    spinlock_t lock;
    printf("%d\n" , __preempt_count);
    spin_lock(&lock);
    printf("%d\n" , __preempt_count);
    spin_unlock(&lock);
    printf("%d\n" , __preempt_count);
    return 0;
}

