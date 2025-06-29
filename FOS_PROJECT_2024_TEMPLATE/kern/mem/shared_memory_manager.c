#include <inc/memlayout.h>
#include "shared_memory_manager.h"

#include <inc/mmu.h>
#include <inc/error.h>
#include <inc/string.h>
#include <inc/assert.h>
#include <inc/queue.h>
#include <inc/environment_definitions.h>

#include <kern/proc/user_environment.h>
#include <kern/trap/syscall.h>
#include "kheap.h"
#include "memory_manager.h"

//==================================================================================//
//============================== GIVEN FUNCTIONS ===================================//
//==================================================================================//
struct Share* get_share(int32 ownerID, char* name);

//===========================
// [1] INITIALIZE SHARES:
//===========================
//Initialize the list and the corresponding lock
void sharing_init()
{
#if USE_KHEAP
	LIST_INIT(&AllShares.shares_list) ;
	init_spinlock(&AllShares.shareslock, "shares lock");
#else
	panic("not handled when KERN HEAP is disabled");
#endif
}

//==============================
// [2] Get Size of Share Object:
//==============================
int getSizeOfSharedObject(int32 ownerID, char* shareName)
{
	//[PROJECT'24.MS2] DONE
	// This function should return the size of the given shared object
	// RETURN:
	//	a) If found, return size of shared object
	//	b) Else, return E_SHARED_MEM_NOT_EXISTS
	//
	struct Share* ptr_share = get_share(ownerID, shareName);
	if (ptr_share == NULL)
		return E_SHARED_MEM_NOT_EXISTS;
	else
		return ptr_share->size;

	return 0;
}

//===========================================================


//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//
//===========================
// [1] Create frames_storage:
//===========================
// Create the frames_storage and initialize it by 0
inline struct FrameInfo** create_frames_storage(int numOfFrames)
{
	//TODO: [PROJECT'24.MS2 - #16] [4] SHARED MEMORY - create_frames_storage()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("create_frames_storage is not implemented yet");
	//Your Code is Here...
	//struct FrameInfo** arrOfFrames={0};
	struct FrameInfo** arrOfFrames = (struct FrameInfo**)kmalloc(numOfFrames * sizeof(struct FrameInfo*));
	int succeed=0;

	if(numOfFrames>0 && arrOfFrames !=NULL){
		for(int i=0;i<numOfFrames;i++){
			struct FrameInfo* pointerFrame=NULL;
			arrOfFrames[i]=pointerFrame;
	  }
//		succeed=1;
		return arrOfFrames;
	}

//	if(succeed==1){
//		return arrOfFrames;
//	}else if(numOfFrames<=0 || arrOfFrames==NULL||succeed==0){
//		return NULL;
//	}

	return NULL;

}

//=====================================
// [2] Alloc & Initialize Share Object:
//=====================================
//Allocates a new shared object and initialize its member
//It dynamically creates the "framesStorage"
//Return: allocatedObject (pointer to struct Share) passed by reference
struct Share* create_share(int32 ownerID, char* shareName, uint32 size, uint8 isWritable)
{
	//TODO: [PROJECT'24.MS2 - #16] [4] SHARED MEMORY - create_share()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("create_share is not implemented yet");
	//Your Code is Here...
	//if(!holding_spinlock(&(AllShares.shareslock)))  acquire_spinlock(&(AllShares.shareslock));

	struct Share* shahd_sharedObject=(struct Share*)kmalloc(sizeof(struct Share));
	if(shahd_sharedObject==NULL){

		//if(holding_spinlock(&(AllShares.shareslock))) release_spinlock(&(AllShares.shareslock));

		return NULL;
	}
	//else if(shahd_sharedObject!=NULL){
	int numOfFrames=ROUNDUP(size,PAGE_SIZE)/PAGE_SIZE; //get the num of frames
	shahd_sharedObject->framesStorage=create_frames_storage(numOfFrames); //create the frame storage and return the arr.
	if(shahd_sharedObject->framesStorage==NULL){
			if(shahd_sharedObject!=NULL){
				kfree(shahd_sharedObject);
			}

			//if(holding_spinlock(&(AllShares.shareslock))) release_spinlock(&(AllShares.shareslock));

			return NULL;
		}
		shahd_sharedObject->isWritable=isWritable;
		shahd_sharedObject->size=size;
		shahd_sharedObject->ownerID=ownerID;
		//shahd_sharedObject->name=shareName;
		strncpy(shahd_sharedObject->name, shareName, 64);
		shahd_sharedObject->references=1;

		// masking
	    //uint32 msbMask = 0x7FFFFFFF;
	    //cprintf("msbMask %d \n ",msbMask);
		int32 va_shared=(int32)shahd_sharedObject;

		// way 1 : to shift left to remove msb and then concatinate the rest of va with 0
		//shahd_sharedObject->ID= 0+(va_shared << 1);
		shahd_sharedObject->ID = (int32)((unsigned long)shahd_sharedObject& 0x7FFFFFFF);
		// cprintf("msbMask %d \n ",shahd_sharedObject->ID);

		// way 2: Anding to remove it
		//shahd_sharedObject->ID= va_shared & msbMask;

//	}


	//UNDO

	if(shahd_sharedObject==NULL){
		if(shahd_sharedObject->framesStorage!=NULL){
			kfree(shahd_sharedObject->framesStorage);
		}
		//if(holding_spinlock(&(AllShares.shareslock))) release_spinlock(&(AllShares.shareslock));

		return NULL;
	}

	//if(holding_spinlock(&(AllShares.shareslock))) release_spinlock(&(AllShares.shareslock));


	return shahd_sharedObject;



}


//=============================
// [3] Search for Share Object:
//=============================
//Search for the given shared object in the "shares_list"
//Return:
//	a) if found: ptr to Share object
//	b) else: NULL
struct Share* get_share(int32 ownerID, char* name)
{
//AllShares.shareslock;
//struct spinlock shareslock;
	//TODO: [PROJECT'24.MS2 - #17] [4] SHARED MEMORY - get_share()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("get_share is not implemented yet");
	//Your Code is Here...
//acquire_spinlock(&AllShares.shareslock);
	//if(!holding_spinlock(&(AllShares.shareslock)))  acquire_spinlock(&(AllShares.shareslock));

	            struct Share* sh;
	          LIST_FOREACH(sh,&AllShares.shares_list){
	    	   if (sh->ownerID == ownerID && strcmp(name,sh->name) == 0)
	        {
	//    		   release_spinlock(&AllShares.shareslock);
	  //  		   if(holding_spinlock(&(AllShares.shareslock))) release_spinlock(&(AllShares.shareslock));


	    		   return sh;
	        }
	    }
	  //        release_spinlock(&AllShares.shareslock);

	//          if(holding_spinlock(&(AllShares.shareslock))) release_spinlock(&(AllShares.shareslock));


	          return NULL;
}

//=========================
// [4] Create Share Object:
//=========================
int createSharedObject(int32 ownerID, char* shareName, uint32 size, uint8 isWritable, void* virtual_address)
{
	if(!holding_spinlock(&(AllShares.shareslock)))  acquire_spinlock(&(AllShares.shareslock));

	 struct Env* myenv = get_cpu_proc();

    // Allocate & Initialize a new share object
   // i will check if a shared object with the same name already exists
    if (get_share(ownerID, shareName) != NULL) {
    	if(holding_spinlock(&(AllShares.shareslock))) release_spinlock(&(AllShares.shareslock));

    	return E_SHARED_MEM_EXISTS;
    }

    // now there is another object with same data so i will create the shared object
    struct Share* sharedobj = create_share(ownerID, shareName, size, isWritable);
    if (sharedobj == NULL) {

    	if(holding_spinlock(&(AllShares.shareslock))) release_spinlock(&(AllShares.shareslock));

    	return E_NO_SHARE;
    }

    uint32 engy_size = ROUNDUP(size, PAGE_SIZE);
   // cprintf("engy_size %d \n ",engy_size );
    uint32 va = ROUNDDOWN((uint32)virtual_address, PAGE_SIZE);
    //cprintf("va : %d \n",va);
    uint32 address = va;
    uint32 numPages = engy_size / PAGE_SIZE;
    //cprintf("numPages : %d \n",numPages);
    int perm = (isWritable) ? (PERM_PRESENT | PERM_WRITEABLE | PERM_MARKED | PERM_USER) : (PERM_PRESENT | PERM_MARKED|!PERM_WRITEABLE | PERM_USER);
   // acquire_spinlock(&(AllShares.shareslock));
    //add new object to list
    LIST_INSERT_TAIL(&AllShares.shares_list, sharedobj);
   // release_spinlock(&(AllShares.shareslock));

    // Allocate ALL required space in the physical memory on a PAGE boundary
    for (uint32 i = 0; i < numPages; i++) {
        struct FrameInfo* frame;
        allocate_frame(&frame);
        int map_res = map_frame(myenv->env_page_directory, frame, va,  PERM_WRITEABLE | PERM_USER|PERM_USED);
        if (map_res != 0) {
            // map failed
        	//for (uint32 i = 0; i < numPages; i++) {
            for (uint32 j = 0; j < i; j++) {

                unmap_frame(myenv->env_page_directory, address);
                sharedobj->framesStorage[j] = NULL;

                address += PAGE_SIZE;
            }

            kfree((void*)sharedobj);
            if(holding_spinlock(&(AllShares.shareslock))) release_spinlock(&(AllShares.shareslock));

            return E_NO_MEM;
        }

        // i will store the frame reference and move to the next virtual address



        sharedobj->framesStorage[i] = frame;
       // cprintf("frame->vasize %d \n ",frame->vasize );

        va += PAGE_SIZE;
    }

    //cprintf("\nShared object created successfully with ID: %d\n", sharedobj->ID);
    if(holding_spinlock(&(AllShares.shareslock))) release_spinlock(&(AllShares.shareslock));

    return sharedobj->ID;
}




//======================
// [5] Get Share Object:
//======================
int getSharedObject(int32 ownerID, char* shareName, void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #21] [4] SHARED MEMORY [KERNEL SIDE] - getSharedObject()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
   //Your Code is Here...
	if(!holding_spinlock(&(AllShares.shareslock)))  acquire_spinlock(&(AllShares.shareslock));

	struct Env* myenv = get_cpu_proc(); //The calling environment


	  struct Share* The_shared_Object = get_share(ownerID, shareName);
	 // cprintf("The_shared_Object->size  %d \n ",The_shared_Object->size );

	  uint32 va=(uint32)virtual_address;
	    if (The_shared_Object == NULL) {
	    	if(holding_spinlock(&(AllShares.shareslock))) release_spinlock(&(AllShares.shareslock));

	    	return E_SHARED_MEM_NOT_EXISTS;
	    }

	    int s=ROUNDUP(The_shared_Object->size,PAGE_SIZE)/PAGE_SIZE;

//	    for (int i = 0;The_shared_Object->framesStorage[i] != NULL; i++) {
//	          //  The_shared_Object->framesStorage[i];
	    for(int i=0;i<s;i++){
	    if(The_shared_Object->isWritable){
	            	map_frame( myenv->env_page_directory,The_shared_Object->framesStorage[i] ,va,PERM_MARKED|PERM_USER|PERM_PRESENT|PERM_WRITEABLE|PERM_USED);
	            	// cprintf("The_shared_Object->framesStorage[ %d ]->vasize   %d \n ",i,The_shared_Object->framesStorage[i]->vasize );

	            }
	            else{
	            	map_frame( myenv->env_page_directory,The_shared_Object->framesStorage[i] ,va,PERM_MARKED|PERM_USER|PERM_PRESENT);
	            //cprintf("The_shared_Object->framesStorage[ %d ]->vasize   %d \n ",i,The_shared_Object->framesStorage[i]->vasize );
	            }


	            va=va+PAGE_SIZE;

	    }

//	    uint32 da = ((int)i - USER_HEAP_START) / PAGE_SIZE; //update user_arr
//	    					user_arr[da].first_va = (uint32)index;
//	    					user_arr[da].size = pages_needed * PAGE_SIZE;
//	    					user_arr[da].allocated = 1;
//

	    The_shared_Object->references+=1;
	    if(holding_spinlock(&(AllShares.shareslock))) release_spinlock(&(AllShares.shareslock));

	    return The_shared_Object->ID;
}



//==================================================================================//
//============================== BONUS FUNCTIONS ===================================//
//==================================================================================//

//==========================
// [B1] Delete Share Object:
//==========================
//delete the given shared object from the "shares_list"
//it should free its framesStorage and the share object itself
// this function to remove a shared memory object from the list
//void remove_share_from_list(struct Share* ptrShare)
//{
//	if(!holding_spinlock(&(AllShares.shareslock)))  acquire_spinlock(&(AllShares.shareslock));
//    LIST_REMOVE(&AllShares.shares_list, ptrShare);
//    if(holding_spinlock(&(AllShares.shareslock))) release_spinlock(&(AllShares.shareslock));
//
//}
/////////////////////////////two/////////////////////////
// this function to free the memory frames associated with a shared memory object
//void free_shared_frames(struct Share* ptrShare)
//{
//    uint32 num_of_frames = ROUNDDOWN(ptrShare->size, PAGE_SIZE) / PAGE_SIZE;
//    for (uint32 i = 0; i < num_of_frames ; i++) {
//        struct FrameInfo* ourframe = ptrShare->framesStorage[i];
//        if (ourframe != NULL) {
//            ourframe->references--;
//            // if the reference count reaches zero,then no other process is using this frame
//            // i will free the physical frame
//            if (ourframe->references == 0) {
//                free_frame(ourframe);
//            }
//            ptrShare->framesStorage[i] = NULL;
//        }
//    }
//}


void free_share(struct Share* ptrShare)
{
	panic("not\n");
//	  acquire_spinlock(&(AllShares.shareslock));
//	  LIST_REMOVE(&AllShares.shares_list, ptrShare);
//	  release_spinlock(&(AllShares.shareslock));
//	   int num_of_frames = ROUNDDOWN(ptrShare->size, PAGE_SIZE) / PAGE_SIZE;
//	   // i will check if the shared object has an associated frame storage
//	    if (ptrShare->framesStorage != NULL)
//	    {
//	    	// this loop is to iterate over the frames in the storage
//	        for (int i = 0; i < num_of_frames; i++)
//	        {
//	            if (ptrShare->framesStorage[i] != NULL)
//	            {
//	            	ptrShare->framesStorage[i]->references--;
//	            	 // if the frame is no longer referenced by any shared object,then i will free it
//	            	 if (ptrShare->framesStorage[i]->references == 0) {
//	            	   free_frame(ptrShare->framesStorage[i]);
//	           }
//	            	  // clear the frame reference from the storage array
//	                 ptrShare->framesStorage[i] = NULL;
//	            }
//	        }
//	        // i will free the memory allocated for the frame storage array
//	        kfree(ptrShare->framesStorage);
//	    }
//	    // fffffffffffffffree the memory allocated for the shared object itself
//	    kfree(ptrShare);
}

//========================
// [B2] Free Share Object:
//========================
int freeSharedObject(int32 sharedObjectID, void *startVA)
{


//    struct Env *myenv = get_cpu_proc();
//    uint32 *ptr_page_table = NULL;
//    uint32 index = PTX((uint32)startVA);
//
//    acquire_spinlock(&(AllShares.shareslock));
//    struct Share* engy_obj = NULL;
//    // for loop to iterate through the shared object list
//    for (engy_obj = AllShares.shares_list.lh_first; engy_obj != NULL; engy_obj = engy_obj->prev_next_info.le_next) {
//        if (engy_obj->ID == sharedObjectID) {
//            break;
//        }
//    }
//    // if the shared object is not found,then i will release the lock and return an error
//    if (engy_obj == NULL) {
//        release_spinlock(&(AllShares.shareslock));
//        return -1;
//    }
//    release_spinlock(&(AllShares.shareslock));
//
//    uint32 startaddress = ROUNDDOWN((uint32)startVA, PAGE_SIZE);
//    uint32 size = ROUNDUP(engy_obj->size, PAGE_SIZE);
//    uint32 endaddress = size + startaddress;
//    // i will iterate through every page in the shared memory
//    for (uint32 i = startaddress; i < endaddress; i += PAGE_SIZE) {
//    	// unmap the frame from the process's page directory
//        unmap_frame(myenv->env_page_directory, i);
//        // invalidate the corresponding entry in the working set
//        env_page_ws_invalidate(myenv, i);
//
//        get_page_table(myenv->env_page_directory, i, &ptr_page_table);
//        // check if the page table is empty
//        if (ptr_page_table != NULL) {
//            bool isempty = 1;
//
//            for (int i = 0; i < NPDENTRIES; i++) {
//                if (ptr_page_table[i] & PERM_PRESENT) {
//                    isempty = 0;
//                    break;
//                }
//            }
//            if (isempty) {
//                kfree(ptr_page_table);
//                myenv->env_page_directory[PDX(i)] = 0;
//            }
//        }
//        // invalidate the TLB entry for the current address
//        tlb_invalidate(myenv->env_page_directory, (void *)i);
//    }
//
//    engy_obj->references--;
//
//    if (engy_obj->references == 0) {
//        free_share(engy_obj);
//    }
//
//    return 0;
	panic("Not\n");
}


