// Sleeping locks

#include "inc/types.h"
#include "inc/x86.h"
#include "inc/memlayout.h"
#include "inc/mmu.h"
#include "inc/environment_definitions.h"
#include "inc/assert.h"
#include "inc/string.h"
#include "sleeplock.h"
#include "channel.h"
#include "../cpu/cpu.h"
#include "../proc/user_environment.h"

void init_sleeplock(struct sleeplock *lk, char *name)
{
	init_channel(&(lk->chan), "sleep lock channel");
	init_spinlock(&(lk->lk), "lock of sleep lock");
	strcpy(lk->name, name);
	lk->locked = 0;
	lk->pid = 0;
}
int holding_sleeplock(struct sleeplock *lk)
{
	int r;
	acquire_spinlock(&(lk->lk));
	r = lk->locked && (lk->pid == get_cpu_proc()->env_id);
	release_spinlock(&(lk->lk));
	return r;
}
//==========================================================================
void acquire_sleeplock(struct sleeplock *lk)
{
	acquire_spinlock(&(lk->lk));
	    while (lk->locked) {
	        sleep(&(lk->chan), &(lk->lk));
	    }
// Mark the sleeplock as locked and record the current process ID
	    lk->locked = 1;
	    lk->pid = get_cpu_proc()->env_id;
// Release the spinlock
	    release_spinlock(&(lk->lk));

}
void release_sleeplock(struct sleeplock *lk) {

    acquire_spinlock(&(lk->lk));

    // Ensure that the sleeplock is currently locked
    assert(lk->locked);

    // Mark the sleeplock as unlocked and clear the process ID
    lk->locked = 0;
    lk->pid = 0;

    // Wake up all processes sleeping on the sleeplock's channel
    wakeup_all(&(lk->chan));

    // Release the spinlock
    release_spinlock(&(lk->lk));
}

