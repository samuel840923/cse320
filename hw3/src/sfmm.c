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
void flipAlloc(void*ptr,uint64_t blck_size ,size_t bit);
int checksize(void* ptr);
/*
stat variable
*/
static int allocated_block;
static int splinter_block;
static uint64_t padding_total;
static uint64_t   splint_total;
static int co_times;
static double total_payload;
static double max_payload;

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
	allocated_block++;
	total_payload+=size;
	padding_total+=padding;

	pt = best_fit(freelist_head, size+padding+16 );

	if(pt==NULL){
		if(freelist_head!=NULL){
			pt = sf_sbrk(0);
			sf_footer check = *((sf_footer*)pt-2);
			if(check.alloc==0){
				uint64_t b = check.block_size<<4;
					if((pt=sf_sbrk(size+padding+16-b))==(void *) -1){
						errno = ENOMEM;
						allocated_block--;
						total_payload-=size;
						padding_total-=padding;
							return NULL;
						}
						 uint64_t  estimate = (unsigned long)sf_sbrk(0)-(unsigned long)pt;
							totalsize+=estimate;
						if( totalsize>(4*4096)){
							errno = ENOMEM;
							allocated_block--;
						total_payload-=size;
						padding_total-=padding;
							return NULL;
						}
						if(max_payload<total_payload)
							max_payload=total_payload;

						uint64_t  alloSize = (unsigned long)sf_sbrk(0)-(unsigned long)pt;
						sf_footer foot = *((sf_footer*)pt-2);
						uint64_t blc = foot.block_size<<4;
						uint64_t offset = blc/sizeof(sf_header);
						pt = ((sf_header*)pt-offset);
						co_times++;
						if(((alloSize+blc)-(size+16+padding))<32){
							uint64_t splint = (alloSize+blc)-(size+16+padding);
							uint64_t s = 1;
							if(splint ==0)
								s=0;
							else
								splinter_block++;

						 splint_total+=splint;
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
		  			   removeFree(pt);
		  			   insertFree(freelist);
		  			   	bl = size+16+padding;
		  			   putBlock(pt,1,0,bl,size,0,padding);
		   			   pt =((sf_header*)pt+1);
						return pt;
					}

				}
			if((pt=sf_sbrk((size+padding+16)))==(void *) -1){
					errno = ENOMEM;
						allocated_block--;
						total_payload-=size;
						padding_total-=padding;
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
						allocated_block--;
						total_payload-=size;
						padding_total-=padding;
						errno = ENOMEM;
							return NULL;
						}

				if(max_payload<total_payload)
				   max_payload=total_payload;
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
			 	else
			 		splinter_block++;

			 	 splint_total+=splint;
			 	putBlock(pt,1,s,bl,size,splint,padding);
			 	removeFree(pt);
			 	pt =((sf_header*)pt+1);
			 	return pt;
			 	}

		}


	else{
		 if(max_payload<total_payload)
		max_payload = total_payload;
		sf_free_header found = *((sf_free_header*)pt);
		 uint64_t newBlock=  (uint64_t)found.header.block_size<<4;

		 uint64_t  expected = size+padding+16;
		 if(newBlock-expected<32){
			 uint64_t bl = newBlock;
			 uint64_t  splint = newBlock-expected;
			 uint64_t s=1;
			 if(splint==0)
			 	s=0;
			 else
			 	splinter_block++;
			  splint_total+=splint;
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
	if(ptr==NULL){
		errno=EINVAL;
		return NULL;
	}
	if(!((unsigned long)start<=(unsigned long)ptr&&(unsigned long)ptr<=(unsigned long)sf_sbrk(0))){
		errno = EINVAL;
			return NULL;
		}
	if(checksize(ptr)==0){
	errno = EINVAL;
	return NULL;
}
	uint64_t splinterHead = (*((sf_header*)ptr-1)).splinter;
	uint64_t splintersize = (*((sf_header*)ptr-1)).splinter_size;
	if((splinterHead==1)&&(splintersize==0)){
		errno = EINVAL;
		return NULL;
	}
	uint64_t checkheader_bit = (*((sf_header*)ptr-1)).alloc;
	uint64_t offe =  (*((sf_header*)ptr-1)).block_size<<4;
	offe = (offe-16)/(sizeof(sf_footer));
	uint64_t checkfoot_bit = (*((sf_footer*)ptr+offe)).alloc;
	uint64_t splinterfoot = (*((sf_footer*)ptr+offe)).splinter;
	if(checkfoot_bit!=1||checkheader_bit!=1){
		errno=EINVAL;
		return NULL;
	}
	if(splinterfoot!=splinterHead){
		errno = EINVAL;
		return NULL;
	}
	if(size==0){
		sf_free(ptr);
		return NULL;
	}
	uint64_t pad = (*((sf_header*)ptr-1)).padding_size;
	uint64_t spll = (*((sf_header*)ptr-1)).splinter_size;
	uint64_t req_size = (*((sf_header*)ptr-1)).requested_size;
	total_payload-=req_size;
	total_payload+=size;
	padding_total-=pad;
	splint_total-=spll;
	if(spll!=0)
	splinter_block--;
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
	padding_total+=padding;
	uint64_t minS = size+padding+16;
	ptr =((sf_header*)ptr-1);
	uint64_t currblck = (*((sf_header*)ptr)).block_size<<4;
	if(minS == currblck){
		 putBlock(ptr,1,0,minS,size,0,padding);
		 ptr =((sf_header*)ptr+1);
		   if(max_payload<total_payload)
		max_payload = total_payload;
		return ptr;
	}
	if(currblck>minS){
		 if(max_payload<total_payload)
		max_payload = total_payload;
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
				else{
					splinter_block++;
				}
				splint_total+=splint;
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
			 	 if(max_payload<total_payload)
					max_payload = total_payload;

			 	uint64_t unused = (nextsize+currblck)-minS;
			 	if(unused<32){
			 		void* nextFree = ((sf_header*)ptr+off);
			 		removeFree(nextFree);
			 		int s =1;
			 		if(unused==0)
			 			s=0;
			 		else
			 			splinter_block++;
			 		splint_total+=unused;
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

		uint64_t content = currblck-16;
		void *freeH = best_fit(freelist_head,minS);
		if(freeH ==NULL){
			uint64_t off_new = currblck/sizeof(sf_header);
			if(checkRight(ptr)==1||((unsigned long)((sf_header*)ptr+off_new))==(unsigned long)sf_sbrk(0)){
				uint64_t requested_size =0;
				if(((unsigned long)((sf_header*)ptr+off_new))==(unsigned long)sf_sbrk(0)){
					 requested_size= size+padding+16-currblck;
					}
				else{
					sf_header nextfreel = *((sf_header*)ptr+off_new);
					uint64_t nextfreeS = nextfreel.block_size<<4;
					uint64_t test_off = nextfreeS+currblck;
					test_off = test_off/sizeof(sf_header);
					if(((unsigned long)((sf_header*)ptr+test_off))==(unsigned long)sf_sbrk(0))
						requested_size = size+padding+16-currblck;
					else
						requested_size = size+padding+16;
					}
			if((freeH=sf_sbrk(requested_size))==(void *) -1){
						errno = ENOMEM;
						splint_total+=spll;
						padding_total+=pad;
						splinter_block++;
						total_payload+=req_size;
						total_payload-=size;
							return NULL;
						}
					}
			else{
			if((freeH=sf_sbrk(size+padding+16))==(void *) -1){
						errno = ENOMEM;
						splint_total+=spll;
						padding_total+=pad;
						splinter_block++;
						total_payload+=req_size;
						total_payload-=size;
							return NULL;
						}
					}
						 uint64_t  estimate = (unsigned long)sf_sbrk(0)-(unsigned long)freeH;
							totalsize+=estimate;
						if( totalsize>(4*4096)){
							errno = ENOMEM;
						splint_total+=spll;
						padding_total+=pad;
						splinter_block++;
						total_payload+=req_size;
						total_payload-=size;
							return NULL;
						}
						if(checkLeft(freeH)==1){
							putBlock(freeH,0,0,estimate,0,0,0);
							insertFree(freeH);
							coal((sf_header*)freeH+1);
							removeFree(freeH);
						}
						else{
							putBlock(freeH,0,0,estimate,0,0,0);
							insertFree(freeH);
						}
						freeH = best_fit(freelist_head,minS);
			if(checkRight(ptr)==1){
							if(max_payload<total_payload)
			    			 max_payload = total_payload;
						uint64_t off = currblck/sizeof(sf_header);
			uint64_t nextsize =  (*((sf_header*)ptr+off)).block_size<<4;
			int c = (nextsize+currblck)-minS;
			 if((c>=0)){

			 	 if(max_payload<total_payload)
					max_payload = total_payload;

			 	uint64_t unused = (nextsize+currblck)-minS;
			 	if(unused<32){
			 		void* nextFree = ((sf_header*)ptr+off);
			 		removeFree(nextFree);
			 		int s =1;
			 		if(unused==0)
			 			s=0;
			 		else
			 			splinter_block++;
			 		splint_total+=unused;
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
			}

			  if(max_payload<total_payload)
			  max_payload = total_payload;

				uint64_t freeblc = ((*(sf_header*)freeH)).block_size<<4;
				if((freeblc-minS)<32){
					uint64_t splint = freeblc-minS;
					int s =1;
			 		if(splint==0)
			 			s=0;
			 		else
			 			splinter_block++;
			 		splint_total+=splint;
			 		removeFree(freeH);
			 		putBlock(freeH,1,s,freeblc,size,splint,padding);
			 		freeH = ((sf_header*)freeH+1);
			 		memcpy(freeH,((void*)((sf_header*)ptr+1)),content);
			 		sf_free(((sf_header*)ptr+1));
			 		allocated_block++;
			 		return freeH;
				}
				uint64_t unused = freeblc-minS;
				uint64_t off = minS/sizeof(sf_header);
				putBlock((sf_header*)freeH+off,0,0,unused,0,0,0);
				insertFree((sf_header*)freeH+off);

				putBlock(freeH,1,0,minS,size,0,padding);
				freeH = (sf_header*)freeH+1;
			 	memcpy(freeH,(void*)((sf_header*)ptr+1),content);
			 	sf_free(((sf_header*)ptr+1));
			 	allocated_block++;
			 	return freeH;


	}

	return NULL;
}

void sf_free(void* ptr) {

if(ptr==NULL){
	errno = EINVAL;
	return;
}
if(!((unsigned long)start<=(unsigned long)ptr&&(unsigned long)ptr<=(unsigned long)sf_sbrk(0))){
errno = EINVAL;
return;
}

sf_header currP = *((sf_header*)ptr-1);
uint64_t header_blocksize_check = currP.block_size<<4;
if(checksize(ptr)==0){
	errno = EINVAL;
	return;
}
if((currP.splinter==1)&&(currP.splinter_size==0)){
	errno = EINVAL;
	return;
}
uint64_t offe =  (*((sf_header*)ptr-1)).block_size<<4;
offe = (offe-16)/(sizeof(sf_footer));
uint64_t checkfoot_bit = (*((sf_footer*)ptr+offe)).alloc;
uint64_t splinter_footer_bit = (*((sf_footer*)ptr+offe)).splinter;
uint64_t checkblock_foot = (*((sf_footer*)ptr+offe)).block_size<<4;
if(currP.alloc==0||checkfoot_bit==0){
	errno = EINVAL;
	return;
}
if(checkblock_foot!=header_blocksize_check){
	errno = EINVAL;
	return;
}
if(splinter_footer_bit!=(currP.splinter)){
	errno = EINVAL;
	return;
}

uint64_t requested = currP.requested_size;
uint64_t pd = currP.padding_size;
uint64_t splll = currP.splinter_size;
padding_total-=pd;
splint_total-=splll;
if(splll!=0)
splinter_block--;
total_payload-=requested;
allocated_block--;
coal(ptr);
// 4 cases
}

int sf_info(info* ptr) {
	if(ptr==NULL){
		return -1;
	}
	double peak = max_payload/totalsize;
	info now= {allocated_block,splinter_block,padding_total,splint_total,co_times,peak};
	*(info*)ptr = now;
	return 0;
}
void* best_fit(sf_free_header *head,size_t dreamS){
void* best = NULL;
sf_free_header *curr = head;
int min =-1;
			while(curr!=NULL){
				size_t curr_size = curr->header.block_size<<4;
				if(dreamS<=curr_size){
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
		//flipAlloc(ptr,block_size,0);
		insertFree(ptr);
	}

	else if(nextP.alloc == 1){
		ptr =((sf_header*)ptr-1);
		putBlock(ptr,0,0,block_size,0,0,0);
		//flipAlloc(ptr,block_size,0);
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
		co_times++;
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
co_times++;


}
else if(checkRight(ptr)==1){
uint64_t blck = (*(sf_header*)ptr).block_size<<4;
uint64_t off = blck/sizeof(sf_header);
uint64_t nxtB = (*((sf_header*)ptr+off)).block_size<<4;
putBlock(ptr,0,0,blck+nxtB,0,0,0);
removeFree((sf_header*)ptr+off);
insertFree(ptr);
co_times++;
}
else if(checkLeft(ptr)==1){
uint64_t currblck = (*((sf_footer*)ptr)).block_size<<4;
uint64_t prevblck = (*((sf_footer*)ptr-2)).block_size<<4;
uint64_t off = prevblck/sizeof(sf_header);
ptr =((sf_header*)ptr-off);
removeFree(ptr);
putBlock(ptr,0,0,currblck+prevblck,0,0,0);
insertFree(ptr);
co_times++;


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
	currA->next->prev=NULL;
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
void flipAlloc(void*ptr,uint64_t blck_size ,size_t bit){


		(*(sf_header*)ptr).alloc= bit;
		 uint64_t blocks = blck_size-8;
		blocks = (blocks)/sizeof(sf_footer);
		(*((sf_footer*)ptr+blocks)).alloc = bit;
}
int checksize(void* ptr){
	void *check = ptr;
	check = (char*)ptr-8;
	sf_header header = *(sf_header*)check;
 	uint64_t spl = header.splinter_size;
 	uint64_t pad = header.padding_size;
 	uint64_t goalblck = header.block_size<<4;
 	uint64_t requested = header.requested_size;

 	uint64_t needed = spl+pad+requested+16;
 	if(needed==goalblck)
 		return 1;
 	else
 		return 0;

}
