#include "kheap.h"

#include <inc/memlayout.h>
#include <inc/dynamic_allocator.h>
#include "memory_manager.h"



#include <kern/conc/sleeplock.h>


//Initialize the dynamic allocator of kernel heap with the given start address, size & limit
//All pages in the given range should be allocated
//Remember: call the initialize_dynamic_allocator(..) to complete the initialization
//Return:
//	On success: 0
//	Otherwise (if no memory OR initial size exceed the given limit): PANIC

struct spinlock  klk;


int initialize_kheap_dynamic_allocator(uint32 daStart, uint32 initSizeToAllocate, uint32 daLimit)
	{
		start_kheep= (uint32*)daStart;
		uint32 initSizeToAllocate_Kheep=ROUNDUP(initSizeToAllocate,PAGE_SIZE);
		Kbrk=(uint32*)(initSizeToAllocate+daStart);
		hard_limit_kheep=(uint32*)daLimit;
				init_spinlock(&klk,"Kheap lock");
//		uint32 end=start_kheep+initSizeToAllocate_Kheep;
		if(start_kheep+initSizeToAllocate_Kheep>hard_limit_kheep)
				{
					panic("Not Memory\n");
				}
		uint32 numofpages=initSizeToAllocate_Kheep / PAGE_SIZE;
		uint32 step=PAGE_SIZE;
		for(uint32 i=0;i<numofpages;i++)
		{
			struct FrameInfo *ptr_frame=NULL;
			allocate_frame(&ptr_frame);
			uint32 ptr=(uint32)start_kheep+(i*step);
			map_frame(ptr_page_directory,ptr_frame,ptr,PERM_WRITEABLE| PERM_PRESENT);
			ptr_frame->va=(uint32*)ptr;
		}
		initialize_dynamic_allocator(daStart,initSizeToAllocate_Kheep);
		return 0;
	}
//===================================================================================================================
/* in kheap.h
================
uint32* start_kheep;
uint32* Kbrk;
uint32* hard_limit_kheep;
*/
void* sbrk(int numOfPages)
{
	/* numOfPages > 0: move the segment break of the kernel to increase the size of its heap by the given numOfPages,
	 * 				you should allocate pages and map them into the kernel virtual address space,
	 * 				and returns the address of the previous break (i.e. the beginning of newly mapped memory).
	 * numOfPages = 0: just return the current position of the segment break
	 *
	 * NOTES:
	 * 	1) Allocating additional pages for a kernel dynamic allocator will fail if the free frames are exhausted
	 * 		or the break exceed the limit of the dynamic allocator. If sbrk fails, kernel should panic(...)
	 */

	//MS2: COMMENT THIS LINE BEFORE START CODING====
	//return (void*)-1 ;
	//====================================================

	//[PROJECT'24.MS2] Implement this function
	// Write your code here, remove the panic and write your code
	//panic("sbrk() is not implemented yet...!!");

	uint32 totalsize=numOfPages*PAGE_SIZE;
	uint32* prev_brkkk=Kbrk;
	uint32* new_brk=(uint32*)(((uint32)Kbrk)+totalsize);
	if(totalsize==0){
		return (void*)prev_brkkk;
	}
	if(totalsize>0 && new_brk<hard_limit_kheep){
		uint32* _brk=Kbrk;
		Kbrk=(uint32*)(((uint32)Kbrk)+totalsize);
		for(uint32 i=0;i<numOfPages;i++)
		{
			struct FrameInfo *ptr_frame=NULL;
			int ret=allocate_frame(&ptr_frame);
			if(ret==E_NO_MEM)
			{return (void*)-1;}
		    int res=map_frame(ptr_page_directory,ptr_frame, (uint32)_brk,PERM_WRITEABLE| PERM_PRESENT);
		    if(res==E_NO_MEM)
		    {return (void*)-1;}
		    ptr_frame->va=(uint32*)_brk;
		    _brk=(uint32*)((uint32)_brk+PAGE_SIZE);

		}
		return (void*)prev_brkkk;
	}
	// if we will exceed the hard limit or the kbrk == hard limit and no memory available to be mapped
	if(new_brk>hard_limit_kheep || Kbrk==hard_limit_kheep){
		return (void*)-1 ;
	}
	return NULL;
}


//TODO: [PROJECT'24.MS2 - BONUS#2] [1] KERNEL HEAP - Fast Page Allocator

///////////****************************kmalllllllllocccccccccc**************
///////////****************************kmalllllllllocccccccccc**************
///////////****************************kmalllllllllocccccccccc**************


#define number_of_pages ((KERNEL_HEAP_MAX - KERNEL_HEAP_START)) / PAGE_SIZE

struct virtual_address_data alloc_arr[number_of_pages] = {0};

int free_allocations = 0;




//struct sleeplock *l;

//struct spinlock *ll;
//bool inlock=0;
void* kmalloc(unsigned int size)
{if(!holding_spinlock(&(klk))) acquire_spinlock(&(klk));

//  init_sleeplock(l,"lk107");
//  acquire_sleeplock(l);
//if(inlock==0)
  //init_spinlock(ll,"lk107");
//  acquire_spinlock(ll);

  uint32 pages_needed = ROUNDUP(size, PAGE_SIZE) / PAGE_SIZE;
  uint32 start_address = (uint32)hard_limit_kheep + PAGE_SIZE;// i will search from start page(start of page allocator) in kernel heap
  uint32 *Page_Table_Ptr = NULL;
  int counter = 0;
struct FrameInfo* ptr_frame_info;
uint32 now_address = 0;//(will be last address)
uint32 index=0;
// i will also check if the size exceed the whole page allocator
//if (start_address + (pages_needed * PAGE_SIZE) > KERNEL_HEAP_MAX) {
//    return NULL;
//}
//first case if i will allocate blocks
if (size <= DYN_ALLOC_MAX_BLOCK_SIZE) {
//	release_sleeplock(l);
	//release_spinlock(ll);

	if(holding_spinlock(&(klk))) release_spinlock(&(klk));
    return (void*)alloc_block_FF(size);
}
//check if it's in page allocator range to search for free pages
else if (size > DYN_ALLOC_MAX_BLOCK_SIZE) {
    if (isKHeapPlacementStrategyFIRSTFIT() == 1) {

    	for (uint32 i = start_address; i < KERNEL_HEAP_MAX; i +=PAGE_SIZE)
{

ptr_frame_info = get_frame_info(ptr_page_directory, i,&Page_Table_Ptr);
if (ptr_frame_info == 0) {
if (counter == 0) {
// i will change the now_address to the current address of first page
now_address = i;
index = i;//(first_va of struct later)
}
counter ++;
if (counter == pages_needed ) {
break;
}
}
//check if we may stop in middle of allocation or complete and if we stoped them i will start from first again as i want consecutive pages
else if (ptr_frame_info != NULL) {
counter = 0;
}
}
}
else {
	//release_sleeplock(l);
	//release_spinlock(ll);

	if(holding_spinlock(&(klk))) release_spinlock(&(klk));
return NULL;
}
}
//now let's check if we found needed pages or not and if i found i will allocate their frames and make the pages point to their frames
if (counter == pages_needed) {
for (uint32 i = 0; i < pages_needed ; i++) {
int ret =allocate_frame(&ptr_frame_info);
if(ret==E_NO_MEM)
			{
//	release_sleeplock(l);
//	release_spinlock(ll);

	if(holding_spinlock(&(klk))) release_spinlock(&(klk));
	return (void*)-1;
			}
int res=map_frame(ptr_page_directory, ptr_frame_info, now_address, PERM_PRESENT | PERM_WRITEABLE);
if(res==E_NO_MEM)
			{
	//release_sleeplock(l);
	//release_spinlock(ll);

	if(holding_spinlock(&(klk))) release_spinlock(&(klk));
	return (void*)-1;}
ptr_frame_info->va = (uint32*)now_address ;
now_address += PAGE_SIZE;
}
uint32 page_num_first = (index - KERNEL_HEAP_START) / PAGE_SIZE;
//we got data about allocated pages
alloc_arr[page_num_first].first_va = index;
alloc_arr[page_num_first].pagesnum = pages_needed;
alloc_arr[page_num_first].size = pages_needed * PAGE_SIZE;
alloc_arr[page_num_first].last_va = now_address;
} else if (counter < pages_needed) {
	//release_sleeplock(l);
//	release_spinlock(ll);

	if(holding_spinlock(&(klk))) release_spinlock(&(klk));
	return NULL;
}
// Return the first allocated address
//release_sleeplock(l);
//release_spinlock(ll);

if(holding_spinlock(&(klk))) release_spinlock(&(klk));
return (void*) (index);
}



//struct spinlock *l;
void kfree(void* virtual_address) {

	if(!holding_spinlock(&(klk))) acquire_spinlock(&(klk));
	//acquire_sleeplock(l);

	 //init_spinlock(l,"lk107");
	//acquire_spinlock(ll);
	uint32 now_address = (uint32)virtual_address;
	uint32 start_address =(uint32) hard_limit_kheep + PAGE_SIZE;
	uint32 start_heap;
	//first case (block allocator)
	if (now_address >= KERNEL_HEAP_START && now_address < (uint32)hard_limit_kheep) {
		free_block(virtual_address);
	}
	//second case(page allocator)
	else if (now_address>= start_address&& now_address < KERNEL_HEAP_MAX) {
	// Free and unmap the consecutive memory pages
		uint32 page_num = (now_address - KERNEL_HEAP_START) / PAGE_SIZE;
		uint32 pagesToFree = alloc_arr[page_num].pagesnum;
		for (int i = 0; i < pagesToFree; i++) {
			page_num = (now_address - KERNEL_HEAP_START) / PAGE_SIZE;
			unmap_frame(ptr_page_directory, now_address);
			//free all data of pages
			alloc_arr[page_num].size = 0;
			alloc_arr[page_num].pagesnum = 0;
			alloc_arr[page_num].first_va = 0;
			alloc_arr[page_num].last_va = 0;
			now_address += PAGE_SIZE;
		}
	}
	else {

		if(holding_spinlock(&(klk))) release_spinlock(&(klk));
		panic("invalid address");
	}
	if(holding_spinlock(&(klk))) release_spinlock(&(klk));

	//release_sleeplock(l);
	//release_spinlock(ll);
}



//
//			///OURCODE
//void* kmalloc(unsigned int size)
//{
////TODO: [PROJECT'23.MS2 - #03] [1] KERNEL HEAP - kmalloc()
//	//refer to the project presentation and documentation for details
//	// use "isKHeapPlacementStrategyFIRSTFIT() ..." functions to check the current strategy
//
//
//uint32 pages_needed = ROUNDUP(size, PAGE_SIZE) / PAGE_SIZE;
//uint32 start_address = (uint32)hard_limit_kheep + PAGE_SIZE;// i will search from start page(start of page allocator) in kernel heap
//uint32 *Page_Table_Ptr = NULL;
//int counter = 0;
//struct FrameInfo* ptr_frame_info;
//uint32 now_address = 0;//(will be last address)
//uint32 index=0;
//// i will also check if the size exceed the whole page allocator
////if (start_address + (pages_needed * PAGE_SIZE) > KERNEL_HEAP_MAX) {
////    return NULL;
////}
////first case if i will allocate blocks
//if (size <= DYN_ALLOC_MAX_BLOCK_SIZE) {
//    return (void*)alloc_block_FF(size);
//}
////check if it's in page allocator range to search for free pages
//else if (size > DYN_ALLOC_MAX_BLOCK_SIZE) {
//    if (isKHeapPlacementStrategyFIRSTFIT() == 1) {
//
//    	for (uint32 i = start_address; i < KERNEL_HEAP_MAX; i +=PAGE_SIZE)
//{
//
//ptr_frame_info = get_frame_info(ptr_page_directory, i,&Page_Table_Ptr);
//if (ptr_frame_info == 0) {
//if (counter == 0) {
//// i will change the now_address to the current address of first page
//now_address = i;
//index = i;//(first_va of struct later)
//}
//counter ++;
//if (counter == pages_needed ) {
//break;
//}
//}
////check if we may stop in middle of allocation or complete and if we stoped them i will start from first again as i want consecutive pages
//else if (ptr_frame_info != NULL) {
//counter = 0;
//}
//}
//}
//else {
//return NULL;
//}
//}
////now let's check if we found needed pages or not and if i found i will allocate their frames and make the pages point to their frames
//if (counter == pages_needed) {
//for (uint32 i = 0; i < pages_needed ; i++) {
//int ret =allocate_frame(&ptr_frame_info);
//if(ret==E_NO_MEM)
//			{return (void*)-1;}
//int res=map_frame(ptr_page_directory, ptr_frame_info, now_address, PERM_PRESENT | PERM_WRITEABLE);
//if(res==E_NO_MEM)
//			{return (void*)-1;}
//ptr_frame_info->va = (uint32*)now_address ;
//now_address += PAGE_SIZE;
//}
//uint32 page_num_first = (index - KERNEL_HEAP_START) / PAGE_SIZE;
////we got data about allocated pages
//alloc_arr[page_num_first].first_va = index;
//alloc_arr[page_num_first].pagesnum = pages_needed;
//alloc_arr[page_num_first].size = pages_needed * PAGE_SIZE;
//alloc_arr[page_num_first].last_va = now_address;
//} else if (counter < pages_needed) {
//return NULL;
//}
//// Return the first allocated address
//return (void*) (index);
//}
//
//
//
//
//void kfree(void* virtual_address) {
//	//TODO: [PROJECT'24.MS2 - #04] [1] KERNEL HEAP - kfree
//		// Write your code here, remove the panic and write your code
//		//panic("kfree() is not implemented yet...!!");
//
//		//you need to get the size of the given allocation using its address
//		//refer to the project presentation and documentation for details
//	uint32 now_address = (uint32)virtual_address;
//	uint32 start_address =(uint32) hard_limit_kheep + PAGE_SIZE;
//	uint32 start_heap;
//	//first case (block allocator)
//	if (now_address >= KERNEL_HEAP_START && now_address < (uint32)hard_limit_kheep) {
//		free_block(virtual_address);
//	}
//	//second case(page allocator)
//	else if (now_address>= start_address&& now_address < KERNEL_HEAP_MAX) {
//	// Free and unmap the consecutive memory pages
//		uint32 page_num = (now_address - KERNEL_HEAP_START) / PAGE_SIZE;
//		uint32 pagesToFree = alloc_arr[page_num].pagesnum;
//		for (int i = 0; i < pagesToFree; i++) {
//			page_num = (now_address - KERNEL_HEAP_START) / PAGE_SIZE;
//			unmap_frame(ptr_page_directory, now_address);
//			//free all data of pages
//			alloc_arr[page_num].size = 0;
//			alloc_arr[page_num].pagesnum = 0;
//			alloc_arr[page_num].first_va = 0;
//			alloc_arr[page_num].last_va = 0;
//			now_address += PAGE_SIZE;
//		}
//	}
//	else {
//		panic("invalid address");
//	}
//}



unsigned int kheap_physical_address(unsigned int virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #05] [1] KERNEL HEAP - kheap_physical_address
	// Write your code here, remove the panic and write your code
	//panic("kheap_physical_address() is not implemented yet...!!");

// Initialize the page table pointer to NULL
uint32 *ptr_page_table = NULL;
struct FrameInfo *ptr_frame_info = NULL;
ptr_frame_info = get_frame_info(ptr_page_directory, virtual_address, &ptr_page_table);
// i will check if the frame info was not found
// If there is no mapping for the virtual address we will return 0
if (ptr_frame_info == NULL) {
    return 0;
}
/////////////////else//////////////////////
// first, i will convert the frame information to its physical address
uint32 physical_address = to_physical_address(ptr_frame_info);

// we will get the offset within the page by deleting the lower 20 bits of the virtual address
uint32 page_offset = virtual_address & 0x00000FFF;
return physical_address + page_offset;

	//return the physical address corresponding to given virtual_address
	//refer to the project presentation and documentation for details

	//EFFICIENT IMPLEMENTATION ~O(1) IS REQUIRED ==================
}



unsigned int kheap_virtual_address(unsigned int physical_address)
{
	//TODO: [PROJECT'24.MS2 - #06] [1] KERNEL HEAP - kheap_virtual_address
	// Write your code here, remove the panic and write your code
	//panic("kheap_virtual_address() is not implemented yet...!!");
	struct FrameInfo *frames_info=to_frame_info(physical_address);
	//cprintf(" \n struct FrameInfo *frames_info=to_frame_info(physical_address); \n");
	uint32 va = (uint32)frames_info->va;
	//uint32 va = frames_info->vvv;
	//cprintf(" \n uint32 va = (uint32)frames_info->va; \n");
	if (frames_info==NULL || va<KERNEL_HEAP_START || va >KERNEL_HEAP_MAX)
		//cprintf(" \n if (!frames_info) \n");
      return 0;
	uint32 offset = physical_address % PAGE_SIZE;
	//cprintf(" \n uint32 offset = physical_address % PAGE_SIZE; \n");
	return va + offset;

	/* uint32 BLOCK_SIZE;
	BLOCK_SIZE= get_block_size((uint32 *) physical_address);
	unsigned int get_block_index(unsigned int physical_address) {
	    return physical_address / BLOCK_SIZE;  // Divide by block size to get the block index
	}
*/

}

//=================================================================================//
//============================== BONUS FUNCTION ===================================//
//=================================================================================//
// krealloc():

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, if moved to another loc: the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to kmalloc().
//	A call with new_size = zero is equivalent to kfree().


void *krealloc(void *virtual_address, uint32 new_size)
{
	   //TODO: [PROJECT'24.MS2 - BONUS#1] [1] KERNEL HEAP - krealloc
	// Write your code here, remove the panic and write your code
	//return NULL;

	//A call with virtual_address = null is equivalent to kmalloc()
	if (virtual_address == NULL)
        return kmalloc(new_size);

    //  A call with new_size = zero is equivalent to kfree()
    else if (new_size == 0) {
        kfree(virtual_address);
        return NULL;
    }

    // we will handle reallocation when the address belongs to the kernel heap.
    else {
        if (virtual_address >= (void*)KERNEL_HEAP_START && virtual_address < (void*)KERNEL_HEAP_MAX) {

            // check if the requested new size exceeds the maximum dynamic block size.
            if (new_size > DYN_ALLOC_MAX_BLOCK_SIZE) {
            	// allocate a new larger block(we will allocate another block with new size and delete the old block)
                void* new_address = kmalloc(new_size);
                if (new_address == NULL)
                    return NULL;
                kfree(virtual_address);
                return new_address;
            }
            else {

                // reallocate within the kernel heap using the FF as kmalloc
                return realloc_block_FF(virtual_address, new_size);
            }
        }
        else {
            // if the address is outside the kernel heap, allocate a new block.
            if (new_size <= DYN_ALLOC_MAX_BLOCK_SIZE) {
            	// Allocate block by FF
                void* new_block = alloc_block_FF(new_size);
                if (new_block == NULL)
                    return NULL;
            }
            else {
                new_size = ROUNDUP(new_size, PAGE_SIZE);
                struct FrameInfo* frame_info;
                uint32* page_table_entry;
                frame_info = get_frame_info(ptr_page_directory, (uint32)virtual_address, &page_table_entry);
                uint32 start_address = (uint32)virtual_address;
                uint32 end_address = start_address + new_size;
                // i will check if the requested range exceeds the kernel heap's maximum limit.
                if (end_address > KERNEL_HEAP_MAX) {
                    void* new_block = kmalloc(new_size);
                    if (new_block == NULL)
                        return NULL;
                    kfree(virtual_address);
                    return new_block;
                }

                for (uint32 i = start_address; i < end_address; i += PAGE_SIZE) {
                    struct FrameInfo* existing_frame;
                    existing_frame = get_frame_info(ptr_page_directory, i, &page_table_entry);
                    if (existing_frame != NULL) {
                        void* new_block = kmalloc(new_size);
                        if (new_block == NULL)
                            return NULL;
                        kfree(virtual_address);
                        return new_block;
                    }
                }

                // allocate additional frames
                for (uint32 i = start_address; i < end_address; i += PAGE_SIZE) {
                    struct FrameInfo* new_frame;
                    allocate_frame(&new_frame);
                    map_frame(ptr_page_directory, new_frame, i, PERM_WRITEABLE | PERM_PRESENT);
                    new_frame->va = (uint32*)i;
                }

                // update the allocation array
                uint32 start_page_index = (start_address - KERNEL_HEAP_START) / PAGE_SIZE;
                alloc_arr[start_page_index].first_va = start_address;
                alloc_arr[start_page_index].pagesnum = (new_size / PAGE_SIZE);
                alloc_arr[start_page_index].size = new_size;
                alloc_arr[start_page_index].last_va = end_address;
            }
        }
    }

    return NULL;
}


