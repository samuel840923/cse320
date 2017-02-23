/**
 * All functions you make for the assignment must be implemented in this file.
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 */
#include "sfmm.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
/**
 * You should store the head of your free list in this variable.
 * Doing so will make it accessible via the extern statement in sfmm.h
 * which will allow you to pass the address to sf_snapshot in a different file.
 */
sf_free_header* freelist_head = NULL;
void putBlock(void* pt, uint64_t allocate_bit,uint64_t splint_bit, uint64_t block_size,uint64_t request, uint64_t splint_size,  uint64_t padding_size );
void* best_fit(sf_free_header *head,size_t deamS);
void insertFree(void* p);
int totalsize;

void *sf_malloc(size_t size) {

	if(size<=0){
		errno = EINVAL;
		return NULL;
	}
	if( totalsize>(4*4096)){
		return NULL;
	}
	void* pt = NULL;
	 uint64_t  padding =0;
	if(size<=16){
		padding = 16-size;
	}
	else {
		padding =size%16;
		if(padding!=0){
			padding = 16-padding;
		}
	}

	pt = best_fit(freelist_head, size+padding+16 );
	printf("%p\n",pt);
	if(pt==NULL){
		if(freelist_head!=NULL){
			pt = sf_sbrk(0);
			sf_footer check = *((sf_footer*)pt-2);
			if(check.alloc==1){
				uint64_t b = check.block_size<<4;
					if((pt=sf_sbrk(size+padding+16-b))==(void *) -1){
						errno = ENOMEM;
							return NULL;
						}
						uint64_t  alloSize = (unsigned long)sf_sbrk(0)-(unsigned long)pt;
						sf_footer foot = *((sf_footer*)pt-2);
						uint64_t blc = foot.block_size<<4;
						uint64_t offset = blc/sizeof(sf_header);
						pt = ((sf_header*)pt-offset);
						if(((alloSize+size)-(size+16+padding))<32){
							uint64_t splint = (alloSize+size)-(size+16+padding);
							uint64_t s = 1;
							if(splint ==0)
								s=0;
						putBlock(pt,1,s,alloSize+size,size,splint,padding);
						pt =((sf_header*)pt+1);
						return pt;
						}

						uint64_t bl = size+16+padding;
						 uint64_t  newDiff = alloSize+size;
						  putBlock(pt,1,0,bl,size,0,padding);
		    			void* freelist = NULL;
		    			freelist = pt;
		    			pt =((sf_header*)pt+1);
		    // now create free list for the too big one

  						 uint64_t  move = size+16+padding;
		   				 move = (move)/sizeof(sf_header);
		  				  freelist =((sf_header*)freelist+move);
		  					 bl = newDiff;
		  			   putBlock(freelist,0,0,bl,0,0,0);
		   			  insertFree(freelist);
						return pt;
					}

				}
			if((pt=sf_sbrk(size+padding+16))==(void *) -1){
					errno = ENOMEM;
					return NULL;
			}
				putBlock(pt,1,0,size+padding+16,size,0,padding);

				void* extraspace = pt;
		 	    uint64_t  estimate = (unsigned long)sf_sbrk(0)-(unsigned long)pt;
				totalsize+=estimate;
				estimate = estimate - (size+padding+16); //free block size
				if(estimate>=32){
		 		uint64_t  move = size+16+padding;
		 		move = (move)/sizeof(sf_header);
				extraspace   =((sf_header*)pt+move);
				putBlock(extraspace,0,0,estimate,0,0,0);
				insertFree(extraspace);
				pt =((sf_header*)pt+1);
				return pt;
				}
			    else{
			 	uint64_t bl = (unsigned long)sf_sbrk(0)-(unsigned long)pt;
			 	uint64_t  splint = bl-(size+16+padding);
			 	uint64_t s = 1;
			 	if(splint==0){
			 		 s = 0;
			 	}
			 	putBlock(pt,1,s,bl,size,splint,padding);
			 	pt =((sf_header*)pt+1);
			 	return pt;
			 	}

		}


	else{
		sf_free_header found = *((sf_free_header*)pt);
		 uint64_t newBlock=  (uint64_t)found.header.block_size<<4;
		 printf("%zu\n",newBlock);
		 uint64_t  expected = size+padding+16;
		 if(newBlock-expected<32){
			 uint64_t bl = newBlock;
			 uint64_t  splint = newBlock-expected;
			 uint64_t s=1;
			 if(splint==0)
			 	s=0;
			 putBlock(pt,1,s,bl,size,splint,padding);

		pt =((sf_header*)pt+1);
		}
		else{
			uint64_t bl = expected;
			 uint64_t  newDiff = newBlock-expected;
			  putBlock(pt,1,0,bl,size,0,padding);
		    void* freelist = NULL;
		    freelist = pt;
		    pt =((sf_header*)pt+1);
		    // now create free list for the too big one

  			 uint64_t  move = size+16+padding;
		    move = (move)/sizeof(sf_header);
		    freelist =((sf_header*)freelist+move);
		    bl = newDiff;
		     putBlock(freelist,0,0,bl,0,0,0);
		     insertFree(freelist);
		}
	}
	return pt;
}

void *sf_realloc(void *ptr, size_t size) {
	return NULL;
}

void sf_free(void* ptr) {
	return;
}

int sf_info(info* ptr) {
	return -1;
}
void* best_fit(sf_free_header *head,size_t dreamS){
void* best = NULL;
sf_free_header *curr = head;
int min =-1;
			while(curr!=NULL){
				size_t curr_size = curr->header.block_size<<4;
				if(dreamS<curr_size){
					int temp = curr_size-dreamS;
					if(min==-1){
						best = curr;
						min =temp;
					}
					else if(min>temp){
						min = temp;
						best = curr;
					}

				}
						curr = curr->next;

				}
return best;
}
void insertFree(void *p){
if(freelist_head == NULL){
	freelist_head = (sf_free_header*)p;
	freelist_head->header = *(sf_header*)p;
	sf_blockprint(freelist_head);
	return;
}
sf_free_header* curr = freelist_head;
sf_free_header* tail = curr;
unsigned long currA = (unsigned long)p;
while (curr!=NULL){
	unsigned long currentA= (unsigned long)curr;
	uint64_t capa = curr->header.block_size<<4;
	unsigned long overlap= (unsigned long)curr+capa;
	if(currentA<=currA&&currentA<overlap){
		if(curr==freelist_head){
			sf_free_header* newB = (sf_free_header*)p;
			newB = (sf_free_header*)p;
			newB->next = freelist_head->next;

			if(freelist_head->next!=NULL)
			freelist_head->next->prev= newB;

			newB->prev=NULL;
			newB->header = *(sf_header*)p;
			freelist_head=newB;
			sf_blockprint(freelist_head);
			return;
		}
		else if(curr->next==NULL){
			sf_free_header* newB = (sf_free_header*)p;
			curr->prev->next = newB;
			newB->prev = curr->prev;
			newB->header=*(sf_header*)p;
			return;
		}
		else{
			sf_free_header* newB = (sf_free_header*)p;
			newB ->header = *(sf_header*)p;
			newB->next = curr->next;
			curr->next->prev =newB;
			newB->prev = curr->prev;
			curr->prev->next= newB;
			return;
		}
	}
	if(currA<currentA){
		if(curr==freelist_head){
		sf_free_header* newB = (sf_free_header*)p;
		newB->header = *(sf_header*)p;
		curr->prev = newB;
		newB ->next = curr;
		freelist_head = newB;
		return;
		}
		else{
			sf_free_header* newB = (sf_free_header*)p;
			newB->header = *(sf_header*)p;
			newB->next = curr->next;
			curr->prev = newB;
			curr->next = newB;
			newB->prev = curr;
			return;
		}

	}
	tail =curr;
	curr = curr->next;
}
curr = (sf_free_header*)p;
curr->header = *(sf_header*)p;
tail->next = curr;
curr->prev = tail;
return ;
}
void putBlock(void* pt, uint64_t allocate_bit,uint64_t splint_bit, uint64_t block_size,uint64_t request, uint64_t splint_size,  uint64_t padding_size ){

		sf_header newBlock = {allocate_bit,splint_bit,(block_size)>>4,request,0,splint_size,padding_size};
		sf_footer newFoot = {allocate_bit,splint_bit,(block_size)>>4};
		*(sf_header*)pt = newBlock;
		 uint64_t blocks = block_size-8;
		blocks = (blocks)/sizeof(sf_footer);
		*((sf_footer*)pt+blocks) = newFoot;
}
