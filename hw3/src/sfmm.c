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
#include <string.h>
/**
 * You should store the head of your free list in this variable.
 * Doing so will make it accessible via the extern statement in sfmm.h
 * which will allow you to pass the address to sf_snapshot in a different file.
 */
sf_free_header* freelist_head = NULL;
void putBlock(void* pt, uint64_t allocate_bit,uint64_t splint_bit, uint64_t block_size,uint64_t request, uint64_t splint_size,  uint64_t padding_size );
void* best_fit(sf_free_header *head,size_t deamS);
void insertFree(void* p);
static int totalsize;
static void* start;
static int checkstart;
void coal(void* ptr);
void removeFree(void*ptr);
int checkRight(void* pt);
int checkLeft(void*pt);
void *sf_malloc(size_t size) {

	if(size<=0){
		errno = EINVAL;
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
			if(check.alloc==0){
				uint64_t b = check.block_size<<4;
					if((pt=sf_sbrk(size+padding+16-b))==(void *) -1){
						errno = ENOMEM;
							return NULL;
						}
						 uint64_t  estimate = (unsigned long)sf_sbrk(0)-(unsigned long)pt;
							totalsize+=estimate;
						if( totalsize>(4*4096)){
							errno = ENOMEM;
							return NULL;
						}
						uint64_t  alloSize = (unsigned long)sf_sbrk(0)-(unsigned long)pt;
						sf_footer foot = *((sf_footer*)pt-2);
						uint64_t blc = foot.block_size<<4;
						uint64_t offset = blc/sizeof(sf_header);
						pt = ((sf_header*)pt-offset);
						if(((alloSize+blc)-(size+16+padding))<32){
							uint64_t splint = (alloSize+blc)-(size+16+padding);
							uint64_t s = 1;
							if(splint ==0)
								s=0;
						putBlock(pt,1,s,alloSize+blc,size,splint,padding);
						removeFree(pt);
						pt =((sf_header*)pt+1);

						return pt;
						}

						 uint64_t bl = size+16+padding;
						 uint64_t  newDiff = (alloSize+blc)-bl;
		    			void* freelist = NULL;
		    			freelist = pt;

		    // now create free list for the too big one

  						 uint64_t  move = size+16+padding;
		   				 move = (move)/sizeof(sf_header);
		  				  freelist =((sf_header*)freelist+move);
		  					 bl = newDiff;
		  			   putBlock(freelist,0,0,bl,0,0,0);
		  			   insertFree(freelist);
		  			   	bl = size+16+padding;
		  			   putBlock(pt,1,0,bl,size,0,padding);
		   			   pt =((sf_header*)pt+1);
						return pt;
					}

				}
			if((pt=sf_sbrk(size+padding+16))==(void *) -1){
					errno = ENOMEM;
					return NULL;
			}
			if(checkstart==0)
			start=pt;
			checkstart=1;

				putBlock(pt,1,0,size+padding+16,size,0,padding);
				void* extraspace = pt;
		 	    uint64_t  estimate = (unsigned long)sf_sbrk(0)-(unsigned long)pt;
				totalsize+=estimate;
				if( totalsize>(4*4096)){
					errno = ENOMEM;
							return NULL;
						}
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
			 	removeFree(pt);
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
			 removeFree(pt);
		pt =((sf_header*)pt+1);
		}
		else{
			uint64_t bl = expected;
			 uint64_t  newDiff = newBlock-expected;
		    void* freelist = NULL;
		    freelist = pt;
		     uint64_t  move = size+16+padding;
		    move = (move)/sizeof(sf_header);
		    freelist =((sf_header*)freelist+move);
		    putBlock(freelist,0,0,newDiff,0,0,0);
		       insertFree(freelist);
		     putBlock(pt,1,0,bl,size,0,padding);
		    pt =((sf_header*)pt+1);
		    // now create free list for the too big on


		}
	}
	return pt;
}

void *sf_realloc(void *ptr, size_t size) {
	if(size==0){
		sf_free(ptr);
	}
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
	uint64_t minS = size+padding+16;
	ptr =((sf_header*)ptr-1);
	uint64_t currblck = (*((sf_header*)ptr)).block_size<<4;
	if(minS == currblck){
		ptr =((sf_header*)ptr+1);
		return ptr;
	}
	if(currblck>minS){
		if(checkRight(ptr)==1){
		 putBlock(ptr,1,0,minS,size,0,padding);
		 void* freelist = ptr;
		 uint64_t off = minS/sizeof(sf_header);
		 freelist = ((sf_header*)freelist+off);
		 putBlock(freelist,0,0,currblck-minS,0,0,0);
		 insertFree(freelist);
		 freelist =((sf_header*)freelist+1);
		 coal(freelist);
		 ptr =((sf_header*)ptr+1);
		 return ptr;
		}
		else{
			if((currblck-minS)<32){
				uint64_t splint = currblck-minS;
				uint64_t spl = 1;
				if(splint==0)
					spl =0;
				 putBlock(ptr,1,spl,currblck,size,splint,padding);
				ptr =((sf_header*)ptr+1);
				return ptr;
			}
			else{
				void* freeblc= ptr;
				 putBlock(ptr,1,0,minS,size,0,padding);
				 uint64_t off = minS/sizeof(sf_header);
				 freeblc = ((sf_header*)freeblc+off);
				 putBlock(freeblc,0,0,currblck-minS,0,0,0);
				 insertFree(freeblc);
				ptr =((sf_header*)ptr+1);
				return ptr;
			}
		}
	}
	else{ //allocate bigger block
		if(checkRight(ptr)==1){
			uint64_t off = currblck/sizeof(sf_header);
			uint64_t nextsize =  (*((sf_header*)ptr+off)).block_size<<4;
			int c = (nextsize+currblck)-minS;
			 if((c>=0)){
			 	uint64_t unused = (nextsize+currblck)-minS;
			 	if(unused<32){
			 		void* nextFree = ((sf_header*)ptr+off);
			 		removeFree(nextFree);
			 		int s =1;
			 		if(unused==0)
			 			s=0;
			 		putBlock(ptr,1,s,nextsize+currblck,size,unused,padding);
			 	ptr =((sf_header*)ptr+1);
				return ptr;
			 	}
			 	void *freelist = ptr;
			 	uint64_t off = minS/sizeof(sf_header);
			 	freelist = ((sf_header*)ptr+off);
			 	putBlock(freelist,0,0,unused,0,0,0);
		 		insertFree(freelist);
		 		putBlock(ptr,1,0,minS,size,0,padding);
		 		ptr =((sf_header*)ptr+1);
				return ptr;
			 }
		}
		void *oldMem = NULL;
		uint64_t content = currblck-16;
		memcpy(oldMem,(void*)((sf_header*)ptr+1),content);
		sf_free(((sf_header*)ptr+1));
		void *freeH = best_fit(freelist_head,minS);
		if(freeH ==NULL){
			if((freeH=sf_sbrk(size+padding+16))==(void *) -1){
						errno = ENOMEM;
							return NULL;
						}
						 uint64_t  estimate = (unsigned long)sf_sbrk(0)-(unsigned long)freeH;
							totalsize+=estimate;
						if( totalsize>(4*4096)){
							errno = ENOMEM;
							return NULL;
						}

		}


	}

	return NULL;
}

void sf_free(void* ptr) {
if(ptr==NULL){
	errno = EINVAL;
	return;
}
sf_header currP = *((sf_header*)ptr-1);
if(currP.alloc==0){
	errno = EINVAL;
	return;
}

coal(ptr);
// 4 cases
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
	freelist_head->next=NULL;
	freelist_head->prev=NULL;

	return;
}
sf_free_header* curr = freelist_head;
sf_free_header* tail = curr;
unsigned long currA = (unsigned long)p;
while (curr!=NULL){
	unsigned long currentA= (unsigned long)curr;
	uint64_t capa = curr->header.block_size<<4;
	unsigned long overlap= currentA+capa;
	if(currentA<=currA&&currA<overlap){
		if(curr==freelist_head){
			sf_free_header* newB = (sf_free_header*)p;
			newB = (sf_free_header*)p;
			newB->next = freelist_head->next;

			if(freelist_head->next!=NULL)
			freelist_head->next->prev= newB;

			newB->prev=NULL;
			newB->header = *(sf_header*)p;
			freelist_head=newB;

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
			newB->prev = curr->prev;
			curr->prev->next=newB;
			curr->prev->next=newB;
			curr->next->prev = newB;
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
			newB->next = curr;
			newB->prev = curr->prev;
			curr->prev->next = newB;
			curr-> prev = newB;

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
void coal(void* ptr){

sf_header currP = *((sf_header*)ptr-1);
if((unsigned long) start ==((unsigned long)((sf_header*)ptr-1))){
	// check next
	uint64_t block_size = currP.block_size<<4;
	uint64_t offset = block_size/sizeof(sf_header);
	sf_header nextP = *((sf_header*)ptr+offset-1);
	if((unsigned long) ((sf_header*)ptr+offset-1)== ((unsigned long)sf_sbrk(0))){
		ptr =((sf_header*)ptr-1);
		putBlock(ptr,0,0,block_size,0,0,0);
		insertFree(ptr);
	}

	if(nextP.alloc == 1){
		ptr =((sf_header*)ptr-1);
		putBlock(ptr,0,0,block_size,0,0,0);
		insertFree(ptr);
		}
	else {
		sf_header nextB = *((sf_header*)ptr+offset-1);

		uint64_t blocksize = nextB.block_size<<4;
		uint64_t newBlc = blocksize+block_size;
		ptr =((sf_header*)ptr-1);
		putBlock(ptr,0,0,newBlc,0,0,0);
		removeFree((sf_header*)ptr+offset);
		insertFree(ptr);
		}

}
else{
ptr =((sf_header*)ptr-1);
if(checkRight(ptr)==1&&checkLeft(ptr)==1){
uint64_t currblck = (*((sf_header*)ptr)).block_size<<4;
uint64_t off = currblck/sizeof(sf_header);
uint64_t nextblc = (*((sf_header*)ptr+off)).block_size<<4;
removeFree(((sf_header*)ptr+off));
uint64_t prevblck = (*((sf_header*)ptr-1)).block_size<<4;
off = prevblck/sizeof(sf_header);
uint64_t total = currblck+prevblck+nextblc;
ptr = ((sf_header*)ptr-off);
removeFree(ptr);
putBlock(ptr,0,0,total,0,0,0);
insertFree(ptr);


}
else if(checkRight(ptr)==1){
uint64_t blck = (*(sf_header*)ptr).block_size<<4;
uint64_t off = blck/sizeof(sf_header);
uint64_t nxtB = (*((sf_header*)ptr+off)).block_size<<4;
putBlock(ptr,0,0,blck+nxtB,0,0,0);
removeFree((sf_header*)ptr+off);
insertFree(ptr);
}
else if(checkLeft(ptr)==1){
uint64_t currblck = (*((sf_footer*)ptr)).block_size<<4;
uint64_t prevblck = (*((sf_footer*)ptr-2)).block_size<<4;
uint64_t off = prevblck/sizeof(sf_header);
ptr =((sf_header*)ptr-off);
removeFree(ptr);
putBlock(ptr,0,0,currblck+prevblck,0,0,0);
insertFree(ptr);


}
else {
	uint64_t block_size = (*(sf_header*)ptr).block_size<<4;
	putBlock(ptr,0,0,block_size,0,0,0);
	insertFree(ptr);
}
}
}
void removeFree(void*ptr){
if(ptr== NULL)
	return;
sf_free_header *currA = (sf_free_header*)ptr;
if(currA->prev==NULL&&currA->next==NULL){
	freelist_head=NULL;
}
else if(currA->prev==NULL){
	freelist_head = currA->next;
}
else if(currA->next==NULL){
	currA->prev->next=NULL;
	currA->prev = NULL;
}
else {
	currA->prev->next=currA->next;
	currA->next->prev= currA->prev;
}
}
int checkRight(void* pt){

uint64_t blck = (*(sf_header*)pt).block_size<<4;
uint64_t off = blck/sizeof(sf_header);
if((unsigned long) sf_sbrk(0) ==((unsigned long)((sf_header*)pt+off)))
return 0;

uint64_t alloc_bit = (*((sf_header*)pt+off)).alloc;
if(alloc_bit==0)
	return 1;
else return 0;


}
int checkLeft(void*pt){
if((unsigned long) start ==((unsigned long)((sf_header*)pt)))
	return 0;
pt = ((sf_header*)pt-1);
uint64_t alc = (*((sf_header*)pt)).alloc;
if(alc==0)
return 1;
else
return 0;

}
