#include <inc/lib.h>

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

//=============================================
// [1] CHANGE THE BREAK LIMIT OF THE USER HEAP:
//=============================================
/*2023*/
void* sbrk(int increment)
{
	return (void*) sys_sbrk(increment);
}

//=================================
// [2] ALLOCATE SPACE IN USER HEAP:
//=================================

//i will take the way as kmalloc but i will just mark the reserved space if i found it
#define number_of_pages ((USER_HEAP_MAX - USER_HEAP_START)) / PAGE_SIZE // just more easier to use later
//struct to contain all data that i want to know about the reserved space

//struct spinlock lock;
//init_spinlock( &lock , "allocfreeFF lock" );
//acquire_spinlock(lock);



// i will use array of struct to manage allocation status
struct user_data user_arr[number_of_pages] = { 0 };

void* malloc(uint32 size)
{
	//TODO: [PROJECT'24.MS2 - #12] [3] USER HEAP [USER SIDE] - malloc()
	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	if (size == 0) return NULL ;
	//==============================================================
	//[PROJECT'24.MS2] [2] USER HEAP - malloc() [User Side]
	// Write your code here, remove the panic and write your code
	//panic("malloc() is not implemented yet...!!");
	//return NULL;
	//Use sys_isUHeapPlacementStrategyFIRSTFIT() and	sys_isUHeapPlacementStrategyBESTFIT()
	//to check the current strategy
	uint32 start_address = USER_HEAP_START + DYN_ALLOC_MAX_SIZE + PAGE_SIZE;
	int pages_needed = ROUNDUP(size, PAGE_SIZE) / PAGE_SIZE;
	int counter = 0;//to count how many pages are free to mark
	uint32 now_address = 0;// it is the current address and i will move it in loop to track all reserved space
	uint32 index = 0;//i will store in it the first address of reserved space
//first case "If size ≤ DYN_ALLOC_MAX_BLOCK_SIZE" so if the size is less than or equal 2kb i will mark blocks
	if (size <= DYN_ALLOC_MAX_BLOCK_SIZE)
	    return  (void *)alloc_block_FF(size);
////////////////////////second case///////////////////////////
else if (size > DYN_ALLOC_MAX_BLOCK_SIZE) {
	if (sys_isUHeapPlacementStrategyFIRSTFIT() == 1)//check if FF startegy or not
	{//search for free consecutive pages to mark it in page allocator(i will start from page allocator to max of heap)
	for (uint32 i = start_address; i < USER_HEAP_MAX; i += PAGE_SIZE)
	{ // check if its reserved or not
	if (user_arr[(i - USER_HEAP_START) / PAGE_SIZE].allocated == 0) {
		if (counter == 0) {
			//so we just found the first page so i will store its virtual address to use in array
now_address = i;
	index = i;

}
		//cprintf("MALLOC counter++; %d \n ",counter);
		counter++;
	if (counter == pages_needed) {
	break;
}
	} else {
		counter = 0;
	i =i+ user_arr[(i - USER_HEAP_START)/ PAGE_SIZE].size - PAGE_SIZE;
	}
}
	} else {
	// Return NULL if it is not FF
	return NULL;
}
}
	if (counter == pages_needed) {
		for(int i=index;i<index+(pages_needed * PAGE_SIZE);i+=PAGE_SIZE)
		{
			uint32 da = (i - USER_HEAP_START) / PAGE_SIZE;
			user_arr[da].first_va = index;
			user_arr[da].size = pages_needed * PAGE_SIZE;
			user_arr[da].allocated = 1;
			//cprintf("MALLOC user_arr[da].size %d \n ",user_arr[da].size);
			//cprintf("MALLOC index %x \n ",index);

		}
		sys_allocate_user_mem(now_address, (pages_needed * PAGE_SIZE));
		//return (void*) (index);
		}else if (counter < pages_needed) {
			return NULL;
		}

	return (void*) (index);

}





//=================================
// [3] FREE SPACE FROM USER HEAP:
//=================================


void free(void* virtual_address)
{
uint32 current_addr107=(uint32)virtual_address;
uint32 start_limit_107=((uint32)(myEnv->engy_hard_limit))+PAGE_SIZE;
if(current_addr107>=USER_HEAP_START&&current_addr107<(uint32)myEnv->engy_hard_limit)
{
	//cprintf("KFREE current_addr107 %d \n ",current_addr107);
free_block(virtual_address);
}
else if(current_addr107>=start_limit_107&&current_addr107<USER_HEAP_MAX)
{
uint32 size_diff= current_addr107-USER_HEAP_START;
//cprintf("KFREE size_diff %d \n ",size_diff );
uint32 index=size_diff/PAGE_SIZE;
int size_107=user_arr[index].size;
//cprintf("KFREE size_107 %d \n ",size_107);
int numofpages_107=size_107/PAGE_SIZE;
int counter=0;
for(int i=current_addr107;i<current_addr107+size_107;i+=PAGE_SIZE)
{
	uint32 size_diff= i-USER_HEAP_START;
	//cprintf("KFREE size_diff %d \n ",size_diff );
	uint32 index=size_diff/PAGE_SIZE;

	//cprintf("index in free Loop %d \n",index);
	user_arr[index].allocated=0;
	user_arr[index].size=0;
	user_arr[index].first_va=0;


}
sys_free_user_mem(current_addr107,size_107);

//cprintf("counter in free loop %d \n",counter);
}
else
{
panic("Envalid Adress");
}

}





//=================================
// [4] ALLOCATE SHARED VARIABLE:
//=================================

void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	if (size == 0) return NULL ;
	//==============================================================
	//TODO: [PROJECT'24.MS2 - #18] [4] SHARED MEMORY [USER SIDE] - smalloc()
	// Write your code here, remove the panic and write your code
	//panic("smalloc() is not implemented yet...!!");
	uint32 start_address = USER_HEAP_START + DYN_ALLOC_MAX_SIZE + PAGE_SIZE;
		int pages_needed = ROUNDUP(size, PAGE_SIZE) / PAGE_SIZE;
		int counter = 0;//to count how many pages are free to mark
		uint32 now_address = 0;// it is the current address and i will move it in loop to track all reserved space
		uint32 index = -1;//i will store in it the first address of reserved space
	//first case "If size ≤ DYN_ALLOC_MAX_BLOCK_SIZE" so if the size is less than or equal 2kb i will mark blocks
	//	if (size <= DYN_ALLOC_MAX_BLOCK_SIZE)
	//	    return alloc_block_FF(size);
	////////////////////////second case///////////////////////////
	//else if (size > DYN_ALLOC_MAX_BLOCK_SIZE)

		if (sys_isUHeapPlacementStrategyFIRSTFIT() == 1)//check if FF startegy or not
		{//search for free consecutive pages to mark it in page allocator(i will start from page allocator to max of heap)
		for (uint32 i = start_address; i < USER_HEAP_MAX; i += PAGE_SIZE)
		{ // check if its reserved or not
		if (user_arr[(i - USER_HEAP_START) / PAGE_SIZE].allocated == 0) {
			if (counter == 0) {
				//so we just found the first page so i will store its virtual address to use in array
	now_address = i;
		index = i;

	}

		//	cprintf("SMALLOC counter  %d \n ",counter);
			counter++;
		if (counter == pages_needed) {
		break;
	}



		} else {
			counter = 0;
		i =i+ user_arr[(i - USER_HEAP_START)/ PAGE_SIZE].size - PAGE_SIZE;
		}
	}

		} else {
		// Return NULL if it is not FF
		return NULL;
	}

		if (counter == pages_needed) {
			uint32 da = (index - USER_HEAP_START) / PAGE_SIZE;
			int done=sys_createSharedObject(sharedVarName,size,isWritable, (uint32*)index );
			//cprintf("SMALLOC done %d \n ",done);
			if(done>=0){
				sys_allocate_user_mem(now_address, (pages_needed * PAGE_SIZE));
				user_arr[da].first_va = index;
				user_arr[da].size = pages_needed * PAGE_SIZE;
				user_arr[da].allocated = 1;
				//cprintf("SMALLOC index %x \n",index);

				return (void*) (index);

		}
		}

		else if (counter < pages_needed) {
				return NULL;
			}
		//cprintf("index %d \n",index);

		//return (void*) (index);

	return NULL;}







//========================================
// [5] SHARE ON ALLOCATED SHARED VARIABLE:
//========================================
void* getVAofNPages(uint32 size,int pages_needed ){

	uint32 start_address = USER_HEAP_START + DYN_ALLOC_MAX_SIZE + PAGE_SIZE;
	int counter = 0;//to count how many pages are free to mark
	uint32 index = 0;//i will store in it the first address of reserved space
	if (sys_isUHeapPlacementStrategyFIRSTFIT() == 1)//check if FF startegy or not
	{
		for (uint32 i = start_address; i < USER_HEAP_MAX; i += PAGE_SIZE)
			{ // check if its reserved or not
			if (user_arr[(i - USER_HEAP_START) / PAGE_SIZE].allocated == 0) {
				//cprintf("pages_needed = %d \n",pages_needed);
				//cprintf("counter = %d \n",counter);
				counter++;
			if (counter == 1) {
			//so we just found the first page so i will store its virtual address to use in array
			index = i;
			cprintf("sget index(counter) %x \n ",index);
			}

						cprintf("pages_needed = %d \n",pages_needed);
						cprintf("counter = %d \n",counter);

			if (counter == pages_needed){
				cprintf("pages_needed = %d \n",pages_needed);
				cprintf("counter = %d \n",counter);
				cprintf("sget index(Pages) %x \n ",index);
				return (void*) (index);
			}


			else if (counter < pages_needed)
				return NULL;

			} else {
				counter = 0;
				i =i+ user_arr[(i - USER_HEAP_START)/ PAGE_SIZE].size - PAGE_SIZE;
				}
		}


			} else {
			// Return NULL if it is not FF
			return NULL;
		}

		return NULL;
}

void updateUserArr(uint32 va , int pagesNum,uint32 size){

	// p# =5 => i need 4 pages 5+4=9 p# =9 exit loop
	//for (int i=da ; i<pagesNum;i++){
	uint32 da = (va - USER_HEAP_START) / PAGE_SIZE;
	//cprintf("da %d \n ",da);
	//cprintf("user_arr[da].allocated %d \n ",user_arr[da].allocated);
	for (int i = da; i < da+pagesNum; i++) {
			   user_arr[i].first_va = va;
				user_arr[i].size = size;
				user_arr[i].allocated = 1;

	}
	//cprintf("user_arr[da].allocated %d \n ",user_arr[da].allocated);


//		for(int i = da;i<pagesNum+da ;i++){
//				user_arr[da].first_va = va;
//				user_arr[da].size = size;
//				user_arr[da].allocated = 1;
//		}

}

struct sgetstruct {
	int32 id;
	uint32 first_address;
};
struct sgetstruct sgetarr [number_of_pages];
void* sget(int32 ownerEnvID, char *sharedVarName)
{
	//TODO: [PROJECT'24.MS2 - #20] [4] SHARED MEMORY [USER SIDE] - sget()

	uint32 size=sys_getSizeOfSharedObject(ownerEnvID,sharedVarName);
	int pages_needed = ROUNDUP(size, PAGE_SIZE) / PAGE_SIZE;

	if(size==E_SHARED_MEM_NOT_EXISTS) return NULL;

	uint32 start_address = USER_HEAP_START + DYN_ALLOC_MAX_SIZE + PAGE_SIZE;
			//int pages_needed = ROUNDUP(size, PAGE_SIZE) / PAGE_SIZE;
			int counter = 0;//to count how many pages are free to mark
			uint32 now_address = 0;// it is the current address and i will move it in loop to track all reserved space
			uint32 index = -1;//i will store in it the first address of reserved space
		//first case "If size ≤ DYN_ALLOC_MAX_BLOCK_SIZE" so if the size is less than or equal 2kb i will mark blocks
		//	if (size <= DYN_ALLOC_MAX_BLOCK_SIZE)
		//	    return alloc_block_FF(size);
		////////////////////////second case///////////////////////////
		//else if (size > DYN_ALLOC_MAX_BLOCK_SIZE)

			if (sys_isUHeapPlacementStrategyFIRSTFIT() == 1)//check if FF startegy or not
			{//search for free consecutive pages to mark it in page allocator(i will start from page allocator to max of heap)
			for (uint32 i = start_address; i < USER_HEAP_MAX; i += PAGE_SIZE)
			{ // check if its reserved or not
			if (user_arr[(i - USER_HEAP_START) / PAGE_SIZE].allocated == 0) {
				if (counter == 0) {
					//so we just found the first page so i will store its virtual address to use in array
		now_address = i;
			index = i;

		}

			//	cprintf("SMALLOC counter  %d \n ",counter);
				counter++;
			if (counter == pages_needed) {
			break;
		}



			} else {
				counter = 0;
			i =i+ user_arr[(i - USER_HEAP_START)/ PAGE_SIZE].size - PAGE_SIZE;
			}
		}

			} else {
			// Return NULL if it is not FF
			return NULL;
		}

			if (counter == pages_needed) {
				uint32 da = (index - USER_HEAP_START) / PAGE_SIZE;
				int obj=sys_getSharedObject(ownerEnvID,sharedVarName,(void*)index);
				if(obj==E_SHARED_MEM_NOT_EXISTS )return NULL;
				//	cprintf("SMALLOC done %d \n ",done);
				//if(done>=0){
					//sys_allocate_user_mem(now_address, (pages_needed * PAGE_SIZE));
					user_arr[da].first_va = index;
					user_arr[da].size = pages_needed * PAGE_SIZE;
					user_arr[da].allocated = 1;
					//cprintf("SMALLOC index %x \n",index);
					sgetarr[da].id=obj;
					sgetarr[da].first_address=index;
					return (void*) (index);

			//}
			}

			else if (counter < pages_needed) {
					return NULL;
				}
			//cprintf("index %d \n",index);

			//return (void*) (index);

		return NULL;
}



//==================================================================================//
//============================== BONUS FUNCTIONS ===================================//
//==================================================================================//

//=================================
// FREE SHARED VARIABLE:
//=================================
//	This function frees the shared variable at the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from main memory then switch back to the user again.
//
//	use sys_freeSharedObject(...); which switches to the kernel mode,
//	calls freeSharedObject(...) in "shared_memory_manager.c", then switch back to the user mode here
//	the freeSharedObject() function is empty, make sure to implement it.

void sfree(void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - BONUS#4] [4] SHARED MEMORY [USER SIDE] - sfree()
		// Write your code here, remove the panic and write your code
		//panic("sfree() is not implemented yet...!!")
	//Find the ID of the shared variable at the given address
	//Call sys_freeSharedObject() to free it
    uint32 startaddress = ROUNDDOWN((uint32)virtual_address, PAGE_SIZE);
    int index = (startaddress - ((uint32)myEnv->engy_hard_limit + PAGE_SIZE)) / PAGE_SIZE;
////////////////////id of object//////////////////////////////
    int32 engy_id = sgetarr[index].id;
    uint32 engy_obj_size = user_arr[index].size;
    int pages_needed = engy_obj_size / PAGE_SIZE;
    // i will free the user memory associated with this shared object
    sys_free_user_mem(startaddress, engy_obj_size);
    // i will free the shared memory object using id and first va
    int result = sys_freeSharedObject(engy_id, (void*)startaddress);
    if (result == 0) {
        for (int i = index; i < index + pages_needed; i++) {
            user_arr[i].allocated = 0;
            sgetarr[i].id = -1;
            sgetarr[i].first_address = 0;
        }
    }
}

//=================================
// REALLOC USER SPACE:
//=================================
//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to malloc().
//	A call with new_size = zero is equivalent to free().

//  Hint: you may need to use the sys_move_user_mem(...)
//		which switches to the kernel mode, calls move_user_mem(...)
//		in "kern/mem/chunk_operations.c", then switch back to the user mode here
//	the move_user_mem() function is empty, make sure to implement it.
void *realloc(void *virtual_address, uint32 new_size)
{
	//[PROJECT]
	// Write your code here, remove the panic and write your code
	panic("realloc() is not implemented yet...!!");
	return NULL;

}


//==================================================================================//
//========================== MODIFICATION FUNCTIONS ================================//
//==================================================================================//

void expand(uint32 newSize)
{
	panic("Not Implemented");

}
void shrink(uint32 newSize)
{
	panic("Not Implemented");

}
void freeHeap(void* virtual_address)
{
	panic("Not Implemented");

}



