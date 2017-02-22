/**
 * All functions you make for the assignment must be implemented in this file.
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 */
#include "sfmm.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
/**
 * You should store the head of your free list in this variable.
 * Doing so will make it accessible via the extern statement in sfmm.h
 * which will allow you to pass the address to sf_snapshot in a different file.
 */
sf_free_header* freelist_head = NULL;
void* best_fit(sf_free_header *head,size_t deamS);
void insertFree(sf_free_header *head, sf_free_header *pass);

void *sf_malloc(size_t size) {
	if(size<=0){
		errno = EINVAL;
		return NULL;
	}
	void* pt = NULL;
	size_t padding =0;
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
	if(pt==NULL){
		if((pt=sf_sbrk(size+padding+16))==(void *) -1){
			errno = ENOMEM;
			return NULL;
		}
		sf_header newBlock = {1,0,(size+padding+16)>>4,size,0,0,padding};
		sf_footer newFoot = {1,0,(size+padding+16)>>4};
		*(sf_header*)pt = newBlock;
		size_t blocks = size+padding+8;
		blocks = (blocks)/sizeof(sf_footer);
		*((sf_footer*)pt+blocks) = newFoot;
		pt =((sf_header*)pt+1);
	}
	else{
		sf_free_header found = *((sf_free_header*)pt);
		size_t newBlock= found.header.block_size;
		size_t expected = size+padding+16;
		if(newBlock-expected<32){
			int bl = newBlock;
			size_t splint = newBlock-expected;
			sf_header newBlock = {1,1,bl>>4,size,0,0,padding};
			sf_footer newFoot = {1,1,bl>>4};
			*(sf_header*)pt = newBlock;
			size_t blocks = size+8+splint;
		    blocks = (blocks)/sizeof(sf_footer);
		*((sf_footer*)pt+blocks) = newFoot;
		pt =((sf_header*)pt+1);
		}
		else{
			int bl = expected;
			size_t newDiff = newBlock-expected;
			sf_header newBlock = {1,1,bl>>4,size,0,0,padding};
			sf_footer newFoot = {1,0,bl>>4};
			*(sf_header*)pt = newBlock;
			size_t blocks = size+8+padding;
		    blocks = (blocks)/sizeof(sf_footer);
		    *((sf_footer*)pt+blocks) = newFoot;
		    void* freelist = NULL;
		    freelist = pt;
		    pt =((sf_header*)pt+1);
		    // now create free list for the too big one

		    size_t move = size+16+padding;
		    move = (move)/sizeof(sf_header);
		    freelist =((sf_header*)pt+move);
		    bl = newDiff;
		    sf_header newBlock2 = {0,0,bl>>4,0,0,0,0};
		    sf_footer newFoot2 = {0,0,bl>>4};
		    *((sf_header*)freelist) = newBlock2;
		    size_t newfoot = (newDiff-8)/sizeof(sf_footer);
		    *((sf_footer*)freelist+newfoot) = newFoot2;

		    sf_free_header newFree;
		    newFree.header = newBlock2;
		    sf_free_header *pass = &newFree;

		    insertFree(freelist_head,pass);

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
				size_t curr_size = curr->header.block_size;
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
void insertFree(sf_free_header *head, sf_free_header *pass){
if(head ==NULL){

}
}
