#include "inc/lib.h"

struct semaphore create_semaphore(char *semaphoreName, uint32 value)
{
	//TODO: [PROJECT'24.MS3 - #02] [2] USER-LEVEL SEMAPHORE - create_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING

	//Your Code is Here...
	struct semaphore sem_206;

	struct __semdata *sdata_206=(struct __semdata*)smalloc(semaphoreName,sizeof(struct __semdata),1);

	//if(sdata==NULL) return NULL;//NO SPACE FROM SMALLOC


	sdata_206->count = value;

	strncpy(sdata_206->name, semaphoreName, 64);

	sys_init_q(&sdata_206->queue);//syscall

	sem_206.semdata=sdata_206;
	return sem_206;
}
struct semaphore get_semaphore(int32 ownerEnvID, char* semaphoreName)
{
	//TODO: [PROJECT'24.MS3 - #03] [2] USER-LEVEL SEMAPHORE - get_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//Your Code is Here...
	struct semaphore sem_206;
	struct __semdata *sdata_206=(struct __semdata*)sget(ownerEnvID, semaphoreName);
	//if(sdata==NULL) return NULL;//NO SPACE FROM SGET
	sem_206.semdata=sdata_206;

	return sem_206;

}

void wait_semaphore(struct semaphore sem)
{
        //TODO: [PROJECT'24.MS3 - #04] [2] USER-LEVEL SEMAPHORE - wait_semaphore
            uint32 key_Mostafa = 1;

            do{
            //xchg((uint32*)key,sem.semdata->lock);

            }while(xchg(&sem.semdata->lock,1)!=0);
            //while (key !=0);
            sem.semdata->count--;
            if (sem.semdata->count < 0) {
            //enqueue(sem.semdata->queue,myEnv);
            sys_queuing(&sem.semdata->queue);
               // myEnv->env_status=ENV_BLOCKED;
            sem.semdata->lock = 0;
            sys_sched();
            }
            sem.semdata->lock = 0;
            //sys_waiting(sem);

}

void signal_semaphore(struct semaphore sem)
{
    //TODO: [PROJECT'24.MS3 - #05] [2] USER-LEVEL SEMAPHORE - signal_semaphore
        uint32 key_Nada = 1;

            do {
            }while(xchg(&sem.semdata->lock,1)!=0);

            //while (key != 0);
            sem.semdata->count++;

            if (sem.semdata->count <= 0) {
                //dequeue(&sem->queue);
                sys_dequeuing(&sem.semdata->queue);
                //myEnv->env_status=ENV_READY;
             }
            sem.semdata->lock=0;
           //sys_signall(sem);

}

int semaphore_count(struct semaphore sem)
{
	return sem.semdata->count;
}
