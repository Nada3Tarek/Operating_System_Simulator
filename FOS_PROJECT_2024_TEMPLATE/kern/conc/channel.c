/*
 * channel.c
 *
 *  Created on: Sep 22, 2024
 *      Author: HP
 */
#include "channel.h"
#include <kern/proc/user_environment.h>
#include <kern/cpu/sched.h>
#include <inc/string.h>
#include <inc/disk.h>

//===============================
// 1) INITIALIZE THE CHANNEL:
//===============================
// initialize its lock & queue
void init_channel(struct Channel *chan, char *name)
{
	strcpy(chan->name, name);
	init_queue(&(chan->queue));
}




void sleep(struct Channel *chan, struct spinlock* lk)
{
			struct Env* blockprocesstk107 =get_cpu_proc();


			acquire_spinlock(&ProcessQueues.qlock);
			release_spinlock(lk);
			blockprocesstk107->env_status=ENV_BLOCKED;
			enqueue(&(chan->queue),blockprocesstk107);
			 sched();
			 release_spinlock(&ProcessQueues.qlock);
			 acquire_spinlock(lk);

}


void wakeup_one(struct Channel *chan)
{
		acquire_spinlock(&ProcessQueues.qlock);
		if(queue_size( &(chan->queue))!=0){
		struct Env *idelettk107 = dequeue(&(chan->queue));
		assert(idelettk107 != NULL);
		sched_insert_ready0(idelettk107);
		}
		release_spinlock(&ProcessQueues.qlock);


}


void wakeup_all(struct Channel *chan)
{
	acquire_spinlock(&ProcessQueues.qlock);
	int qsize=queue_size( &(chan->queue));
	for(int i=0;i<qsize;i++)
	{

				struct Env *idelettk107 = dequeue(&(chan->queue));
				assert(idelettk107 != NULL);
				sched_insert_ready0(idelettk107);


	}
	release_spinlock(&ProcessQueues.qlock);
}
