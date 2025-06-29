


/*
 * dynamic_allocator.c
 *
 *  Created on: Sep 21, 2023
 *      Author: HP
 */
#include <inc/assert.h>
#include <inc/string.h>
#include "../inc/dynamic_allocator.h"


//==================================================================================//
//============================== GIVEN FUNCTIONS ===================================//
//==================================================================================//

//=====================================================
// 1) GET BLOCK SIZE (including size of its meta data):
//=====================================================
__inline__ uint32 get_block_size(void* va)
{
	uint32 *curBlkMetaData = ((uint32 *)va - 1) ;
	return (*curBlkMetaData) & ~(0x1);
}

//===========================
// 2) GET BLOCK STATUS:
//===========================
__inline__ int8 is_free_block(void* va)
{
	uint32 *curBlkMetaData = ((uint32 *)va - 1) ;
	return (~(*curBlkMetaData) & 0x1) ;
}

//===========================
// 3) ALLOCATE BLOCK:
//===========================

void *alloc_block(uint32 size, int ALLOC_STRATEGY)
{
	void *va = NULL;
	switch (ALLOC_STRATEGY)
	{
	case DA_FF:
		va = alloc_block_FF(size);
		break;
	case DA_NF:
		va = alloc_block_NF(size);
		break;
	case DA_BF:
		va = alloc_block_BF(size);
		break;
	case DA_WF:
		va = alloc_block_WF(size);
		break;
	default:
		cprintf("Invalid allocation strategy\n");
		break;
	}
	return va;
}

//===========================
// 4) PRINT BLOCKS LIST:
//===========================

void print_blocks_list(struct MemBlock_LIST list)
{
	cprintf("=========================================\n");
	struct BlockElement* blk ;
	cprintf("\nDynAlloc Blocks List:\n");
	LIST_FOREACH(blk, &list)
	{
		cprintf("(size: %d, isFree: %d)\n", get_block_size(blk), is_free_block(blk)) ;
	}
	cprintf("=========================================\n");

}
//
////********************************************************************************//
////********************************************************************************//

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

bool is_initialized = 0;
//==================================
// [1] INITIALIZE DYNAMIC ALLOCATOR:
//==================================

void initialize_dynamic_allocator(uint32 daStart, uint32 initSizeOfAllocatedSpace)
{
	//==================================================================================
	//DON'T CHANGE THESE LINES==========================================================
	//==================================================================================
	{
		if (initSizeOfAllocatedSpace % 2 != 0) initSizeOfAllocatedSpace++; //ensure it's multiple of 2
		if (initSizeOfAllocatedSpace == 0)
			return ;
		is_initialized = 1;
	}
	//==================================================================================
	//==================================================================================

	//TODO: [PROJECT'24.MS1 - #04] [3] DYNAMIC ALLOCATOR - initialize_dynamic_allocator
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("initialize_dynamic_allocator is not implemented yet");
	//Your Code is Here...

	struct BlockElement *saddB=(struct BlockElement*) ((uint32 *)daStart);
	uint32 *BEGnadablock , *ENDahmedblock;
	BEGnadablock =(uint32 *)daStart;//beg block beg from the start address

	ENDahmedblock=(uint32 *)(daStart+  initSizeOfAllocatedSpace - sizeof(int));//end block starts after start address + all size -beg block size
	* BEGnadablock = * ENDahmedblock= PERM_PRESENT;//All of it blocks size=0 but allocated with 1
	// cprintf("BEG_block val = %d \n",*BEGnadablock);

	 //cprintf("END_block val = %d \n",*ENDahmedblock);

	//cprintf("BEG / END Block DONE! ");


	struct BlockElement* first_free_block =(struct BlockElement*)(daStart +8);//1st block address start after the given start address with the size of beg block and header of 1st block
	//cprintf("first_free_block = DONE! \n");
	first_free_block->prev_next_info.le_prev=NULL;
	//cprintf("first_free_block prev DONE! \n");
	first_free_block->prev_next_info.le_next=NULL;
	//cprintf("first_free_block next DONE!\n ");


	uint32* firstnnnHeader=(uint32*)( daStart+sizeof(int));//1st block header start after beg block
	uint32* firstaaaFooter=(uint32*)(daStart + initSizeOfAllocatedSpace -(2*sizeof(int)));//1st block footer start after all the space but - his size and end block size
	//cprintf("1st header / footer Block place DONE! \n");

	* firstnnnHeader=initSizeOfAllocatedSpace - (2*sizeof(int));//size of 1st block include it's header and footer (all space-beg &end space)
	* firstaaaFooter=initSizeOfAllocatedSpace - (2*sizeof(int));

	//set_block_data(first_free_block,  initSizeOfAllocatedSpace-(2*sizeof(int)), 0);
	//cprintf("first_free_block set DONE! ");
	LIST_INSERT_HEAD( &freeBlocksList,first_free_block );//insert it in the free Blocks List
	//cprintf("first_free_block list insert DONE! ");
}



//==================================
// [2] SET BLOCK HEADER & FOOTER:
//==================================

void set_block_data(void* va, uint32 totalSize, bool isAllocated)
{
	//TODO: [PROJECT'24.MS1 - #05] [3] DYNAMIC ALLOCATOR - set_block_data
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("set_block_data is not implemented yet");
	//Your Code is Here...

	uint32* header_shahd207=(uint32*)((uint32)va -sizeof(int));
	uint32* footer_shahd207=(uint32*)((uint32)va + totalSize -2*sizeof(int));
	if(isAllocated){
		*header_shahd207= totalSize | PERM_PRESENT ;
		*footer_shahd207= totalSize | PERM_PRESENT;

	}else{
		*header_shahd207= totalSize & ~PERM_PRESENT;
	    *footer_shahd207= totalSize & ~PERM_PRESENT;

	}


}


//=========================================
// [3] ALLOCATE BLOCK BY FIRST FIT:
//=========================================




void *alloc_block_FF(uint32 size)
{
	//==================================================================================
	//DON'T CHANGE THESE LINES==========================================================
	//==================================================================================
	//{
		if (size % 2 != 0) size++;	//ensure that the size is even (to use LSB as allocation flag)
		if (size < DYN_ALLOC_MIN_BLOCK_SIZE)
			size = DYN_ALLOC_MIN_BLOCK_SIZE ;
		if (!is_initialized)
		{
		uint32 required_size = size + 2*sizeof(int) /*header & footer*/ + 2*sizeof(int) /*da begin & end*/ ;
			uint32 da_start = (uint32)sbrk(ROUNDUP(required_size, PAGE_SIZE)/PAGE_SIZE);
			uint32 da_break = (uint32)sbrk(0);
			initialize_dynamic_allocator(da_start, da_break - da_start);
		}

	//==================================================================================
	//==================================================================================

	//TODO: [PROJECT'24.MS1 - #06] [3] DYNAMIC ALLOCATOR - alloc_block_FF
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("alloc_block_FF is not implemented yet");
	//Your Code is Here...

	uint32 metadatasize_shahd207=8;
	uint32 totalsize_shahd207=metadatasize_shahd207+size;
	int found_shahdblk=0;
	int sbrk_done=0;

	if(size==0){
		return 0;
	}
	if(totalsize_shahd207>=16){
	  struct BlockElement* blk_shahd207 ;

	  LIST_FOREACH(blk_shahd207, &freeBlocksList)
	  {
		  if(get_block_size(blk_shahd207)>=totalsize_shahd207){
			  found_shahdblk=1;
			  if(get_block_size(blk_shahd207)==totalsize_shahd207){
				   set_block_data( blk_shahd207,  totalsize_shahd207, 1);
			  }else if(get_block_size(blk_shahd207)>=totalsize_shahd207 && (get_block_size(blk_shahd207)-totalsize_shahd207)<16){
				  set_block_data( blk_shahd207, get_block_size(blk_shahd207) , 1);
			  }else if(get_block_size(blk_shahd207)>=totalsize_shahd207 && (get_block_size(blk_shahd207)-totalsize_shahd207)>=16){
				  uint32  remainspace207_shahd=get_block_size(blk_shahd207)-totalsize_shahd207;
				  uint32* pointer_new_block207=(uint32*) ((uint32)blk_shahd207+(totalsize_shahd207));
				  set_block_data(blk_shahd207, totalsize_shahd207 , 1);
				 struct BlockElement* newblk_shahd207 =(struct BlockElement*)pointer_new_block207;
				 LIST_INSERT_AFTER(&freeBlocksList,blk_shahd207,newblk_shahd207);
				 set_block_data( newblk_shahd207, remainspace207_shahd , 0);
			  }
			  LIST_REMOVE(&freeBlocksList,blk_shahd207);
			 return blk_shahd207;
		  }
	  }
	  // if we need to use sbrk because we don't have blk to fit the space
	  if(found_shahdblk==0){
		int numOf_pages=(ROUNDUP(totalsize_shahd207,PAGE_SIZE)/PAGE_SIZE);
		uint32* brk=(uint32*)sbrk(numOf_pages); // returned brk pointer
		//cprintf("  brk  %p \n",brk);
		if(brk== (uint32*)-1){
			return NULL;  // if can't allocate pages then return null
		}else{
			// if sbrk returned the brk pointer then we can allocate
			sbrk_done=1;
			// end pointer in the address before brk pointer
			uint32* end_blk=(uint32*)((uint32)brk - sizeof(int));

			// we will allocate the new space at the address of prev end blk
			struct BlockElement* new_blk_sbrk=(struct BlockElement*)((uint32)end_blk + sizeof(int));
			end_blk=(uint32*)((uint32)end_blk + numOf_pages*PAGE_SIZE);
			* end_blk= PERM_PRESENT;
			// move the end block with a total size space
			// prev block in the block allocator // the last one before the end
			uint32* footer_of_prev=(uint32*)((uint32) new_blk_sbrk-(sizeof(int)*2));
		    uint32 prev_block_size=((*footer_of_prev >> 1 ) << 1);
			uint32* va_prev_pointer=(uint32*)((uint32) new_blk_sbrk-(prev_block_size));
			// check if prev blk is free
			if(is_free_block(va_prev_pointer)==1){ // if it is free so we will merge
				uint32 newSize=(numOf_pages*PAGE_SIZE)+get_block_size(LIST_LAST(&freeBlocksList));
				set_block_data(LIST_LAST(&freeBlocksList),newSize,0); // the prev blk's size increased
				new_blk_sbrk=LIST_LAST(&freeBlocksList);
			}
			else
			{
				if(new_blk_sbrk!=NULL){
				LIST_INSERT_TAIL(&freeBlocksList,new_blk_sbrk);
				set_block_data(new_blk_sbrk,numOf_pages*PAGE_SIZE,0);
			 }
			}
			if(sbrk_done==1){ // will allocate at the block we made which is last element in the free list
			  struct BlockElement* allocated_block=new_blk_sbrk;
			  uint32 remain_size_sbrk=get_block_size(new_blk_sbrk)-totalsize_shahd207 ; // to see if i will allocate the whole blk or i will split
			  if(remain_size_sbrk==0){ // will allocate the whole blk
				  set_block_data(allocated_block,totalsize_shahd207,1);
			 }else{ // will split
				 uint32* pointer_new_split=(uint32*)((uint32)allocated_block+totalsize_shahd207);
				 set_block_data(allocated_block,totalsize_shahd207,1);
                 LIST_REMOVE(&freeBlocksList,new_blk_sbrk);
                 LIST_INSERT_TAIL(&freeBlocksList,allocated_block);
                 if(remain_size_sbrk>=16){
                	 struct BlockElement* blk_remained=(struct BlockElement*)pointer_new_split;
                	 LIST_INSERT_AFTER(&freeBlocksList,allocated_block,blk_remained);
                	 set_block_data( blk_remained, remain_size_sbrk , 0);
                 }
			}
			  LIST_REMOVE(&freeBlocksList,allocated_block);
			  return allocated_block;
			}
		}
	  }
	}
	return NULL;}



//=========================================
// [4] ALLOCATE BLOCK BY BEST FIT:
//=========================================



void *alloc_block_BF(uint32 size)
{
	//TODO: [PROJECT'24.MS1 - BONUS] [3] DYNAMIC ALLOCATOR - alloc_block_BF
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("alloc_block_BF is not implemented yet");
	//Your Code is Here...

#define UINT32_MAX 0xFFFFFFFF


	if (size % 2 != 0) size++;	//ensure that the size is even (to use LSB as allocation flag)
			if (size < DYN_ALLOC_MIN_BLOCK_SIZE)
				size = DYN_ALLOC_MIN_BLOCK_SIZE ;
			 if (size == 0) return NULL;

			if (!is_initialized)
			{
				uint32 requiredBMWsize = size + 2*sizeof(int) /*header & footer*/ + 2*sizeof(int) /*da begin & end*/ ;
				uint32 daBMWstart = (uint32)sbrk(ROUNDUP(requiredBMWsize, PAGE_SIZE)/PAGE_SIZE);
				uint32 daBMWbreak = (uint32)sbrk(0);
				initialize_dynamic_allocator(daBMWstart, daBMWbreak - daBMWstart);
			}


	uint32 metadatasize=8;
	uint32 totalsize=metadatasize+size;
	int engy=0;

    struct BlockElement *bfb;
    uint32 smallestDiff = UINT32_MAX;
    struct BlockElement *CurrB=LIST_FIRST(&freeBlocksList);
    //acquire_spinlock(&lk);

    LIST_FOREACH(CurrB, &freeBlocksList) {
        if ( is_free_block(CurrB)&&get_block_size(CurrB) >= totalsize) {
            uint32 diff = get_block_size(CurrB) - totalsize;
            if (diff < smallestDiff) {
                smallestDiff = diff;
                bfb = CurrB;
                engy=1;
            }
        }
        CurrB=LIST_NEXT(CurrB);
    }

    if (engy==0) {
	  sbrk(totalsize);

	 	 // release_spinlock(&lk);
	  return NULL;}


    // Case3: If the found block is large enough, split it if possible
    uint32 bestFitSize = get_block_size(bfb);
    if (bestFitSize >= totalsize + 16) {
       uint32* pointer_new_block=(uint32*) ((uint32)bfb+(totalsize));
       struct BlockElement *residualBlock = (struct BlockElement*) pointer_new_block;

        uint32 residualSize = bestFitSize - totalsize;
        set_block_data(bfb, totalsize,1);
        set_block_data(residualBlock, residualSize, 0);
        LIST_INSERT_AFTER(&freeBlocksList, bfb, residualBlock);


    }
   		 else if(get_block_size(bfb)>=totalsize && (get_block_size(bfb)-totalsize)<16){
    			set_block_data( bfb, get_block_size(bfb) , 1);}


LIST_REMOVE(&freeBlocksList,bfb);

	  //release_spinlock(&lk);
return bfb ;



}



//===================================================
// [5] FREE BLOCK WITH COALESCING:
//===================================================
void sortListBYSS206(struct BlockElement*blk)
{
	if(LIST_FIRST(&freeBlocksList)>blk) LIST_INSERT_HEAD(&freeBlocksList,blk); //if the address of blk before address of first element
	else if(LIST_LAST(&freeBlocksList)<blk) LIST_INSERT_TAIL(&freeBlocksList,blk);//address of blk after the list tail
	else if(LIST_FIRST(&freeBlocksList)<blk&&LIST_LAST(&freeBlocksList)>blk){//blk between 2 blks
		struct BlockElement*targetblkss206=LIST_FIRST(&freeBlocksList);
		LIST_FOREACH(targetblkss206,&freeBlocksList){
			if(LIST_NEXT(targetblkss206)>blk&&targetblkss206<blk){
				LIST_INSERT_AFTER( &freeBlocksList,targetblkss206,blk);
				return;}
			//targetblk=LIST_NEXT(targetblk);
		}
			};
}
void free_block(void *va)
{
	//TODO: [PROJECT'24.MS1 - #07] [3] DYNAMIC ALLOCATOR - free_block
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("free_block is not implemented yet");
	//Your Code is Here...
	//struct BlockElement*b =va;
	//struct BlockElement *addr;
	    uint32* footerOfPrevss206=(uint32*)((uint32) va-(sizeof(int)*2)); //to access you prev block access footer with - your header and prev block footer
	    uint32 prevBlockSizess206=((*footerOfPrevss206 >> 1 ) << 1);//from footer get size with shift the allocated bit and increase 1 another bit to git 32 bit => size only
	    uint32* vaPrevpointerss206=(uint32*)((uint32) va-(prevBlockSizess206));// - your start address from your prev size to get pointer on the prev block u want
		if (va==NULL)return;

		struct BlockElement* blkSS206 =(struct BlockElement*)va;
		struct BlockElement* prev_BlkSS206 =(struct BlockElement*)vaPrevpointerss206;//perv addr
		struct BlockElement* next_BlkSS206 =(struct BlockElement*)((uint32) va+get_block_size(blkSS206));//next addr => my addr + my size

		 //acquire_spinlock(&lk);

		if(is_free_block(blkSS206)==0){//if not free
		 // if(is_free_block(prev_blk)==0 && is_free_block(next_blk)==0){
			  //uint32* footer=(uint32*)((uint32) (blk+get_block_size(blk)-(sizeof(int)*2)));
			// uint32 blksize= ((uint32*) footer >> 1 ) << 1);
			   set_block_data(blkSS206,get_block_size(blkSS206),0);
			  // uint32 size =get_block_size(blk);
			   //cprintf("get_block_size(blk)",size);
			   sortListBYSS206(blkSS206);//give it to sort function to sort it in freeblockslist
		  //}

		if( LIST_PREV(blkSS206)==prev_BlkSS206&&LIST_NEXT(blkSS206)!=next_BlkSS206){ //if your prev in list = your prev in address & your next in list != your next in address
			 uint32 blocks_totalsize=get_block_size(blkSS206)+get_block_size(LIST_PREV(blkSS206));//get the sum of u block and u prev block
			set_block_data(LIST_PREV(blkSS206),blocks_totalsize,0);//set your prev with new data
			//sortList(prev_blk);
			//LIST_INSERT_BEFORE( &freeBlocksList,blk,prev_blk);
			LIST_REMOVE(&freeBlocksList,blkSS206);//remove U from the list

				 // release_spinlock(&lk);
			}
		else if(LIST_PREV(blkSS206)!=prev_BlkSS206&&LIST_NEXT(blkSS206)==next_BlkSS206){//if your prev in list != your prev in address & your next in list = your next in address
			//	 set_block_data(blk,get_block_size(blk),0);
			uint32 blocks_totalsize=get_block_size(blkSS206)+get_block_size(LIST_NEXT(blkSS206));//get the sum of u block and u next block
			set_block_data(blkSS206,blocks_totalsize,0);//set you with new data
			//LIST_INSERT_AFTER( &freeBlocksList,blk,next_blk);
			  //sortList(blk);
			   LIST_REMOVE(&freeBlocksList,next_BlkSS206);//remove your next block from the list
			   //release_spinlock(&lk);

		}
		else if(LIST_NEXT(blkSS206)==next_BlkSS206 && LIST_PREV(blkSS206)==prev_BlkSS206){//if your prev in list = your prev in address & your next in list = your next in address
			   // set_block_data(blk,get_block_size(blk),0);
			  uint32 blocks_totalsize=get_block_size(blkSS206)+get_block_size(prev_BlkSS206)+get_block_size(next_BlkSS206);//get the sum of your block , your prev block and your next block
			  set_block_data(prev_BlkSS206,blocks_totalsize,0);//set your prev with new data

			   LIST_REMOVE(&freeBlocksList,blkSS206);//remove your block from the list
			   LIST_REMOVE(&freeBlocksList,next_BlkSS206);//remove your next block from the list
			   //release_spinlock(&lk);

		}



 }



}
//=========================================
// [6] REALLOCATE BLOCK BY FIRST FIT:
//=========================================


void *realloc_block_FF(void* va, uint32 new_size)
{
	struct BlockElement* blk_tk107 =(struct BlockElement*)va;


	if(va!=NULL && new_size==0){
			free_block(va);

			return NULL;
		}
	else if(va==NULL && new_size==0){
		return NULL;
	}

	else if(va==NULL && new_size!=0){
		void* returnAddress=alloc_block_FF(new_size);
		return returnAddress;
	}
	else{
		uint32 old_size_tk107 =get_block_size(blk_tk107);
    	uint32 new_size_f_tk107=new_size;
		new_size = new_size + 2*sizeof(int);

			if(old_size_tk107>new_size)
			{
				uint32 size_freetk107= old_size_tk107-new_size;
				if(size_freetk107>=16)
				{

					set_block_data(va,new_size,1);
					uint32 ptrtk107=(uint32)(va)+new_size;
					struct BlockElement* next_free_blk_tk107 =(struct BlockElement*)ptrtk107;
					 set_block_data( next_free_blk_tk107,size_freetk107,1);

					free_block( next_free_blk_tk107);
					 return va;
				}
				else
				{
					return va;
				}
			}
			else if(new_size>old_size_tk107)
			{
				 uint32 ptr=(uint32)(va)+old_size_tk107;
				 struct BlockElement* next_blk_tk107 =(struct BlockElement*)ptr;
				int8 result107 = is_free_block(next_blk_tk107);
				if(!result107)
				{
					free_block(va);
					void* returnAddress=alloc_block_FF(new_size_f_tk107);

					return returnAddress;
				}
				else
				{
					uint32 next_blk_size=get_block_size(next_blk_tk107);
//					cprintf("\ntotalnextblk:%d\n",next_blk_size);
					uint32 space_to_need107= new_size - old_size_tk107;
//					cprintf("\ntotal:%d\n",total_size);
					if(space_to_need107 <= next_blk_size)
					{


						uint32 free_to_space=next_blk_size -space_to_need107;
						if(free_to_space>=16)
						{
							set_block_data(va,new_size,1);
							uint32 ptr=(uint32)(va)+new_size;
							 struct BlockElement* next_free_blk =(struct BlockElement*)ptr;
							 set_block_data( next_free_blk,free_to_space,0);
							 free_block( next_free_blk);
							 return va;
						}
						else
						{
							uint32 allspace_tk107=next_blk_size+old_size_tk107;
							//acquire_spinlock(&lk);

							LIST_REMOVE(&freeBlocksList,next_blk_tk107);
							 //release_spinlock(&lk);
							set_block_data(va,allspace_tk107,1);

							return va;
						}

					}
					else
					{
						free_block(va);
						void* returnAddress=alloc_block_FF(new_size_f_tk107);


						return returnAddress;
					}


					}

				}

			else
			{
				return va;
			}
				}
			return NULL;

			}






/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
//=========================================
// [7] ALLOCATE BLOCK BY WORST FIT:
//=========================================
void *alloc_block_WF(uint32 size)
{
	panic("alloc_block_WF is not implemented yet");
	return NULL;
}

//=========================================
// [8] ALLOCATE BLOCK BY NEXT FIT:
//=========================================
void *alloc_block_NF(uint32 size)
{
	panic("alloc_block_NF is not implemented yet");
	return NULL;
}













