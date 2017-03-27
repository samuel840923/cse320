#include <criterion/criterion.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "sfmm.h"

/**
 *  HERE ARE OUR TEST CASES NOT ALL SHOULD BE GIVEN STUDENTS
 *  REMINDER MAX ALLOCATIONS MAY NOT EXCEED 4 * 4096 or 16384 or 128KB
 */

Test(sf_memsuite, Malloc_an_Integer, .init = sf_mem_init, .fini = sf_mem_fini) {
  int *x = sf_malloc(sizeof(int));
  *x = 4;
  cr_assert(*x == 4, "Failed to properly sf_malloc space for an integer!");
}

Test(sf_memsuite, Free_block_check_header_footer_values, .init = sf_mem_init, .fini = sf_mem_fini) {
  void *pointer = sf_malloc(sizeof(short));
  sf_free(pointer);
  pointer = (char*)pointer - 8;
  sf_header *sfHeader = (sf_header *) pointer;
  cr_assert(sfHeader->alloc == 0, "Alloc bit in header is not 0!\n");
  sf_footer *sfFooter = (sf_footer *) ((char*)pointer + (sfHeader->block_size << 4) - 8);
  cr_assert(sfFooter->alloc == 0, "Alloc bit in the footer is not 0!\n");
}

Test(sf_memsuite, SplinterSize_Check_char, .init = sf_mem_init, .fini = sf_mem_fini){
  void* x = sf_malloc(32);
  void* y = sf_malloc(32);
  (void)y;

  sf_free(x);

  x = sf_malloc(16);

  sf_header *sfHeader = (sf_header *)((char*)x - 8);
  cr_assert(sfHeader->splinter == 1, "Splinter bit in header is not 1!");
  cr_assert(sfHeader->splinter_size == 16, "Splinter size is not 16");

  sf_footer *sfFooter = (sf_footer *)((char*)sfHeader + (sfHeader->block_size << 4) - 8);
  cr_assert(sfFooter->splinter == 1, "Splinter bit in header is not 1!");
}

Test(sf_memsuite, Check_next_prev_pointers_of_free_block_at_head_of_list, .init = sf_mem_init, .fini = sf_mem_fini) {
  int *x = sf_malloc(4);
  memset(x, 0, 0);
  cr_assert(freelist_head->next == NULL);
  cr_assert(freelist_head->prev == NULL);
}

Test(sf_memsuite, Coalesce_no_coalescing, .init = sf_mem_init, .fini = sf_mem_fini) {
  int *x = sf_malloc(4);
  int *y = sf_malloc(4);
  memset(y, 0, 0);
  sf_free(x);

  //just simply checking there are more than two things in list
  //and that they point to each other
  cr_assert(freelist_head->next != NULL);
  cr_assert(freelist_head->next->prev != NULL);
}

//#
//STUDENT UNIT TESTS SHOULD BE WRITTEN BELOW
//DO NOT DELETE THESE COMMENTS
//#
Test(sf_memsuite, Coalesce_with_coalescing_to_the_right, .init = sf_mem_init, .fini = sf_mem_fini) {
  int *x = sf_malloc(32);
  int *y = sf_malloc(32);
  int *z = sf_malloc(32);
  *z = 3;
  sf_free(y);
  sf_free(x);
   info pt = {0,0,0,0,0,0};
  info *ptr = &pt;
  sf_info(ptr);
  cr_assert(ptr->padding==0,"no need to pad");
  cr_assert(ptr->allocatedBlocks==1,"only one is currently allocated");
  cr_assert(ptr->splinterBlocks==0, "no splint");
  cr_assert(ptr->splintering==0,"no splinter need");
  cr_assert(ptr->coalesces==1, "only one coal");
  cr_assert(freelist_head->header.block_size<<4==96,"48+48=96");
  cr_assert(freelist_head->next->header.block_size<<4==3952,"4096-96-48");
}
Test(sf_memsuite, Coalesce_with_coalescing_to_the_left, .init = sf_mem_init, .fini = sf_mem_fini) {
  int *x = sf_malloc(32);
  int *y = sf_malloc(32);
  int *z = sf_malloc(32);
  *z = 3;
  sf_free(x);
  sf_free(y);
   info pt = {0,0,0,0,0,0};
  info *ptr = &pt;
  sf_info(ptr);
  cr_assert(ptr->padding==0,"no need to pad");
  cr_assert(ptr->allocatedBlocks==1,"only one is currently allocated");
  cr_assert(ptr->splinterBlocks==0, "no splint");
  cr_assert(ptr->splintering==0,"no splinter need");
   cr_assert((*((sf_header*)z-1)).splinter_size==0,"no splinter need");
  cr_assert(ptr->coalesces==1, "only one coal");
  cr_assert(freelist_head->header.block_size<<4==96,"48+48=96");
  cr_assert(freelist_head->next->header.block_size<<4==3952,"4096-96-48");
}
Test(sf_memsuite, Coalesce_with_coalescing_to_both_side, .init = sf_mem_init, .fini = sf_mem_fini) {
  int *x = sf_malloc(32);
  int *y = sf_malloc(32);
  int *z = sf_malloc(32);
  int *a = sf_malloc(32);
  int *b = sf_malloc(32);
  *x = 3;
  *b =4;
  sf_free(y);
  sf_free(a);
  sf_free(z);
   info pt = {0,0,0,0,0,0};
  info *ptr = &pt;
  sf_info(ptr);
  cr_assert(ptr->padding==0,"no need to pad");
  cr_assert(ptr->allocatedBlocks==2,"only 2 is currently allocated");
  cr_assert(ptr->splinterBlocks==0, "no splint");
  cr_assert(ptr->splintering==0,"no splinter need");
  cr_assert(ptr->coalesces==1, "only one coal");
  cr_assert(freelist_head->header.block_size<<4==144,"48+48+48=96");
  cr_assert(freelist_head->next->header.block_size<<4==3856,"4096-96-48");
  cr_assert(*x==3,"x shouldnt be touch");
  cr_assert(*b==4,"b shouldnt be touch");
}
Test(sf_memsuite, Special_Case_freeing_twice, .init = sf_mem_init, .fini = sf_mem_fini) {
  int *x = sf_malloc(32);
  int *y = sf_malloc(32);
  *y=1;
  sf_free(x);
  int* check = sf_realloc(x,100);

  cr_assert(check==NULL,"you using a free space, should return null");
}
Test(sf_memsuite, realloc_to_smaller_space_with_splinter, .init = sf_mem_init, .fini = sf_mem_fini) {
  int *x = sf_malloc(32);
  int *y = sf_malloc(48);
  int *z = sf_malloc(48);
  sf_realloc(y,32);
  *y=10;
  *x=1;
  *z=1;
  info pt = {0,0,0,0,0,0};
  info *ptr = &pt;
  sf_info(ptr);
  cr_assert(ptr->splinterBlocks==1, "you have %zu splint",ptr->splinterBlocks);
  cr_assert(ptr->splintering==16,"should have 16 splinter");
  cr_assert(ptr->allocatedBlocks==3,"only 3 is currently allocated");
  cr_assert((*((sf_header*)y-1)).splinter_size==16,"no splinter need");
  cr_assert((*((sf_header*)y-1)).block_size<<4==64,"wrong block size should be %zu",(*((sf_header*)y-1)).block_size);
  cr_assert(*y==10,"should not change y");

}
Test(sf_memsuite, realloc_to_smaller_space_without_splinter, .init = sf_mem_init, .fini = sf_mem_fini) {
  int *x = sf_malloc(32);
  int *y = sf_malloc(48);
  int *z = sf_malloc(48);
  sf_realloc(y,16);
  *y=10;
  *x=1;
  *z=1;
  info pt = {0,0,0,0,0,0};
  info *ptr = &pt;
  sf_info(ptr);
  cr_assert(ptr->splinterBlocks==0, "you have %zu splint",ptr->splinterBlocks);
  cr_assert(ptr->splintering==0,"should have 16 splinter");
  cr_assert(ptr->allocatedBlocks==3,"only 3 is currently allocated");
  cr_assert(*y==10,"should not change y");
  cr_assert(freelist_head->header.block_size<<4 ==32,"should create a freelist of 32");
  cr_assert(ptr->coalesces==0, "no coal");
}
Test(sf_memsuite, realloc_to_larger_space_with_no_space_after_with_splinter, .init = sf_mem_init, .fini = sf_mem_fini) {
  int *x = sf_malloc(14);
  int *y = sf_malloc(25);
  int *w = sf_malloc(48);
  int *z = sf_malloc(48);
  sf_free(w);
  *x=109;
  void *freelist_head_check = (sf_header*)x-1;
  int *moved = sf_realloc(x,20);
  *y=10;
  *x=1;
  *z=1;
  info pt = {0,0,0,0,0,0};
  info *ptr = &pt;
  sf_info(ptr);
  cr_assert(ptr->splinterBlocks==1, "you have %zu splint",ptr->splinterBlocks);
  cr_assert(ptr->splintering==16,"should have 16 splinter");
  cr_assert(ptr->allocatedBlocks==3,"only 3 is currently allocated");
  cr_assert(*moved==109,"should not change y");
  cr_assert((*((sf_header*)moved-1)).padding_size==12,"should be 12");
  cr_assert((unsigned long)freelist_head == (unsigned long)freelist_head_check,"old spot should be head of freelist");
}
Test(sf_memsuite, realloc_to_larger_with_space_after, .init = sf_mem_init, .fini = sf_mem_fini) {
  int *x = sf_malloc(31);
  int *y = sf_malloc(12);
  int *z = sf_malloc(48);
  int *w = sf_malloc(48);
  *y=10;
  *x=1;
  *w=1;
   sf_free(z);
  void* test= sf_realloc(y,20);
  test = (sf_header*)test-1;

  info pt = {0,0,0,0,0,0};
  info *ptr = &pt;
  sf_info(ptr);
  cr_assert(ptr->splinterBlocks==0, "you have %zu splint",ptr->splinterBlocks);
  cr_assert(ptr->splintering==0,"should have 0 splinter");
  cr_assert(ptr->allocatedBlocks==3,"only 3 is currently allocated");
  cr_assert(*y==10,"should not change y");
  cr_assert(freelist_head->header.block_size<<4 ==48,"should create a freelist of 48");
  cr_assert(ptr->coalesces==0, "0 coal");
  cr_assert((*((sf_header*)test)).block_size<<4==48, "new realloc should be 48");
  cr_assert((*((sf_header*)test)).padding_size==12, "12 padding needed");
   cr_assert(ptr->padding==13,"total padding is 13");

}
Test(sf_memsuite, realloc_to_larger_with_space_after_2, .init = sf_mem_init, .fini = sf_mem_fini) {
  int *x = sf_malloc(31);
  int *y = sf_malloc(12);
  int *z = sf_malloc(45);
  *z=10;
  *x=1;
  *y=1;
  void* test= sf_realloc(z,80);
  int * check = test;
  test = (sf_header*)test-1;

  info pt = {0,0,0,0,0,0};
  info *ptr = &pt;
  sf_info(ptr);
  cr_assert(ptr->splinterBlocks==0, "you have %zu splint",ptr->splinterBlocks);
  cr_assert(ptr->splintering==0,"should have 16 splinter");
  cr_assert(ptr->allocatedBlocks==3,"only 3 is currently allocated");
  cr_assert(*check==10,"should not change z");
  cr_assert(freelist_head->header.block_size<<4 ==3920,"should create a freelist of 3920");
  cr_assert(freelist_head->next==NULL,"should only have one free list");
  cr_assert(ptr->coalesces==0, "0 coal");
  cr_assert((*((sf_header*)test)).block_size<<4==96, "new realloc should be 96");
  cr_assert((*((sf_header*)test)).padding_size==0, "0 padding needed");
   cr_assert(ptr->padding==5,"total padding is 5");
    cr_assert((*((sf_header*)x-1)).block_size<<4==48,"total padding is 5");

}
Test(sf_memsuite, two_coale, .init = sf_mem_init, .fini = sf_mem_fini){

  int *x = sf_malloc(31);
  int *y = sf_malloc(12);
  int *z = sf_malloc(45);
  int *a = sf_malloc(31);
  int *b = sf_malloc(12);
  int *c = sf_malloc(45);
  int *d = sf_malloc(45);
  *a=10;
   *d=10;
  *x=1;
  *c=1;
  sf_free(y);
  sf_free(z);
  sf_free(c);
  sf_free(b);
  info pt = {0,0,0,0,0,0};
  info *ptr = &pt;
  sf_info(ptr);
  cr_assert(ptr->coalesces==2, "should be 2 coal you have %zu coal\n",ptr->coalesces);
  cr_assert(freelist_head->header.block_size<<4 ==96,"should create a freelist of 96");
  cr_assert(freelist_head->next->header.block_size<<4 ==96,"should create a freelist of 96");
  cr_assert(freelist_head->next->next!=NULL,"should have three in freelist");

}
Test(sf_memsuite, best_fit_test, .init = sf_mem_init, .fini = sf_mem_fini){

  int *x = sf_malloc(10);
  int *y = sf_malloc(34);
  int *z = sf_malloc(68);
  int *a = sf_malloc(20);
  int *b = sf_malloc(67);
  int *c = sf_malloc(45);
  int *d = sf_malloc(45);
  int *should_use = d;
  int *h = sf_malloc(45);
  *a=10;
  *y=10;
  *x=1;
  *c=1;
  *h=8;
  sf_free(x);
  sf_free(z);
  sf_free(b);
  sf_free(d);
  int *insert = sf_malloc(20);
  cr_assert(insert==should_use,"should use the last one");
}

Test(sf_memsuite, invalid_free, .init = sf_mem_init, .fini = sf_mem_fini){
int *x = sf_malloc(10);
 int *y = sf_malloc(34);
 sf_free(x);
 sf_free(x);
 *y =3;
 cr_assert(errno==EINVAL,"errno should be set");
}
Test(sf_memsuite, invalid_free_2, .init = sf_mem_init, .fini = sf_mem_fini){
int *a = sf_malloc(10);
int *b = sf_malloc(34);
int *c = sf_malloc(29);
int *d = sf_malloc(89);
sf_free(c);
sf_realloc(a,17);
sf_free(a);
 *b=3;
 *d=3;
 cr_assert(errno==EINVAL,"errno should be set");
}
Test(sf_memsuite, too_many_request_page, .init = sf_mem_init, .fini = sf_mem_fini){
int *a = sf_malloc(4000);
int *b = sf_malloc(4000);
int *c = sf_malloc(4000);
int *d = sf_malloc(4000);
int *e = sf_malloc(4000);
*a=1;
*b=2;
*c=3;
*d=4;
cr_assert(errno==ENOMEM,"errno should be set");
cr_assert(e==NULL,"should return null");
}
Test(sf_memsuite, too_many_request_page_2, .init = sf_mem_init, .fini = sf_mem_fini){
int *a = sf_malloc(4*4096);
cr_assert(a==NULL,"should return null");
cr_assert(errno==ENOMEM,"errno should be set");
}
Test(sf_memsuite, big_case, .init = sf_mem_init, .fini = sf_mem_fini){
int *a = sf_malloc(4000);
int *b = sf_malloc(4000);
int *c = sf_malloc(4000);
*a = 1;
*b = 2;
*c = 3;
 info pt = {0,0,0,0,0,0};
  info *ptr = &pt;
  sf_info(ptr);
  cr_assert(ptr->coalesces==2, "no coal");
   cr_assert((*((sf_header*)b-1)).block_size<<4==4016, "no same");
   cr_assert((*((sf_header*)b-1)).alloc==1, "no same");
  cr_assert(freelist_head->header.block_size<<4==240, "should have free of 240");
  cr_assert(freelist_head->next==NULL, "only one free");
  cr_assert(freelist_head->header.alloc==0, "is free!");
  cr_assert(ptr->splinterBlocks==0, "you have %zu splint",ptr->splinterBlocks);
  cr_assert(ptr->splintering==0,"should have 0 splinter");
   cr_assert(ptr->padding==0,"total padding is 0");
}
Test(sf_memsuite, realloc_big_case, .init = sf_mem_init, .fini = sf_mem_fini){
int *a = sf_malloc(65);
int *b = sf_malloc(93);
sf_realloc(a,4000);
*b=5;
 cr_assert(freelist_head->header.block_size<<4==96, "should have free of 96");
 cr_assert(freelist_head->header.alloc==0, "is free!");
 cr_assert(freelist_head->next->header.block_size<<4==3968, "should have free of 120 but you have %zu\n",freelist_head->next->header.block_size<<4);
 info pt = {0,0,0,0,0,0};
  info *ptr = &pt;
  sf_info(ptr);
  cr_assert(ptr->coalesces==1, "1 coal");
}
Test(sf_memsuite, crazy_coal, .init = sf_mem_init, .fini = sf_mem_fini){
int *a = sf_malloc(4000);
int *b = sf_malloc(4000);
int *c = sf_malloc(4000);
*a=3;
*b=4;
sf_free (c);
cr_assert(freelist_head->header.block_size<<4==4256, "should have free of 120 but you have");
int *d = sf_malloc(23);
int *e = sf_malloc(129);
int *huge = sf_malloc(2000);
sf_free (e);
sf_free (d);
sf_free(huge);
info pt = {0,0,0,0,0,0};
info *ptr = &pt;
sf_info(ptr);
cr_assert(ptr->coalesces==5, "how much should coal %zu",ptr->coalesces);
cr_assert(freelist_head->header.block_size<<4==4256, "how much should coal %zu",freelist_head->header.block_size<<4);
cr_assert(freelist_head->header.alloc==0, "is free!");
cr_assert(freelist_head->next==NULL, "only one freehead (next)");
cr_assert(freelist_head->prev==NULL, "only one freehead (prev)");
}
Test(sf_memsuite, null_freelist, .init = sf_mem_init, .fini = sf_mem_fini){
 int *b = sf_malloc(2032);
int *a = sf_malloc(2032);
*a = 2;
*b =1;
cr_assert(freelist_head==NULL, "no need for freelist");
}
Test(sf_memsuite, null_freelist_2, .init = sf_mem_init, .fini = sf_mem_fini){
 int *b = sf_malloc(8176);
*b =1;
cr_assert(freelist_head==NULL, "no need for freelist");
}
Test(sf_memsuite, invalid_pointer_realloc, .init = sf_mem_init, .fini = sf_mem_fini){
int a =3;
int *inv = &a;
int *re = sf_realloc(inv,100);
cr_assert(re==NULL, "is invalid pointer!!");
cr_assert(errno==EINVAL, "errno is not set proper");
}
Test(sf_memsuite, invalid_pointer_free, .init = sf_mem_init, .fini = sf_mem_fini){
int a =3;
sf_free(&a);
cr_assert(errno==EINVAL, "errno is not set proper");
}
Test(sf_memsuite, peak_utilization_test, .init = sf_mem_init, .fini = sf_mem_fini){
int *a = sf_malloc(298);
int *b = sf_malloc(478);
int *c = sf_malloc(988);
int *d = sf_malloc(250);
int *e = sf_malloc(123);
sf_realloc(a,0);
sf_free(b);
*d=123;
*e = 321;
*c=12313;
int *moved = sf_realloc(c,1098);
 info pt = {0,0,0,0,0,0};
 info *ptr = &pt;
 sf_info(ptr);
cr_assert(freelist_head->header.block_size<<4==1824,"is not 1824");
cr_assert(freelist_head->next->header.block_size<<4==736,"is not 736");
cr_assert(freelist_head->header.alloc==0, "is free!");
cr_assert(freelist_head->next->header.alloc==0, "is free!");
 cr_assert(ptr->coalesces==2, "2 coal");
 cr_assert(*moved==12313, "shouldnt change");
cr_assert(ptr->peakMemoryUtilization==(2137.0/4096),"is not equal" );
void* allc = moved;
 allc = (char*)allc- 8;
  sf_header *sfHeader = (sf_header *)allc;
 cr_assert(sfHeader->alloc == 1, "Alloc bit in header is not 1\n");
 cr_assert(sfHeader->block_size<<4 == 1120, "Alloc block size in header is not 1120\n");
sf_footer *sfFooter = (sf_footer *) ((char*)allc + (sfHeader->block_size << 4) - 8);
 cr_assert(sfFooter->alloc == 1, "Alloc bit in the footer is not 1!\n");
}
Test(sf_memsuite, invalid_header_footer_free, .init = sf_mem_init, .fini = sf_mem_fini){
  sf_header header_test= {1,1,48,21,0,11,0};
  void *erro = sf_malloc(21);
  erro = (char*)erro- 8;
  *(sf_header*)erro = header_test;
   erro = (char*)erro+ 8;
  sf_free(erro);
  cr_assert(errno==EINVAL,"errno should be set");
}
Test(sf_memsuite, invalid_header_footer_realloc, .init = sf_mem_init, .fini = sf_mem_fini){
  sf_header header_test= {1,1,48,21,0,11,0};
  void *erro = sf_malloc(21);
  erro = (char*)erro- 8;
  *(sf_header*)erro = header_test;
   erro = (char*)erro+ 8;
  void* test = sf_realloc(erro,200);
  cr_assert(errno==EINVAL,"errno should be set");
  cr_assert(test==NULL,"errno should be set");
}
Test(sf_memsuite, invalid_header_footer_realloc_2, .init = sf_mem_init, .fini = sf_mem_fini){
  sf_header header_test= {1,0,48,21,0,11,0};
  sf_footer footer_test = {1,1,48};
  void *erro = sf_malloc(21);
  erro = (char*)erro- 8;
  *(sf_header*)erro = header_test;
  uint64_t block_s = (*(sf_header*)erro).block_size<<4;
  uint64_t off = (block_s-8)/sizeof(sf_footer);
  (*((sf_footer*)erro+off))= footer_test;
   erro = (char*)erro+ 8;
  void* test = sf_realloc(erro,200);
  cr_assert(errno==EINVAL,"errno should be set");
  cr_assert(test==NULL,"errno should be set");
}
Test(sf_memsuite, invalid_header_footer_realloc_3, .init = sf_mem_init, .fini = sf_mem_fini){
  sf_header header_test= {1,0,48,21,0,11,0};
  sf_footer footer_test = {1,0,48};
  void *erro = sf_malloc(21);
  erro = (char*)erro- 8;
  *(sf_header*)erro = header_test;
  uint64_t block_s = (*(sf_header*)erro).block_size<<4;
  uint64_t off = (block_s-8)/sizeof(sf_footer);
  (*((sf_footer*)erro+off))= footer_test;
   erro = (char*)erro+ 8;
  void* test = sf_realloc(erro,200);
  cr_assert(errno==EINVAL,"errno should be set");
  cr_assert(test==NULL,"errno should be set");
}
Test(sf_memsuite, realloc_to_smaller_space_with_splinter_merging, .init = sf_mem_init, .fini = sf_mem_fini) {
  int *x = sf_malloc(32);
  int *y = sf_malloc(48);
  int *z = sf_malloc(48);
  sf_realloc(z,32);
  *y=10;
  *x=1;
  *z=1;
  info pt = {0,0,0,0,0,0};
  info *ptr = &pt;
  sf_info(ptr);
  cr_assert(ptr->splinterBlocks==0, "you have %zu splint",ptr->splinterBlocks);
  cr_assert(ptr->splintering==0,"should have 16 splinter");
  cr_assert(ptr->allocatedBlocks==3,"only 3 is currently allocated");
  cr_assert((*((sf_header*)z-1)).splinter_size==0,"no splinter need");
  cr_assert(freelist_head->header.block_size<<4==3936,"wrong you have %zu",freelist_head->header.block_size<<4);
  cr_assert((*((sf_header*)z-1)).block_size<<4==48,"wrong block size should be %zu",(*((sf_header*)y-1)).block_size);
  cr_assert(*y==10,"should not change y");

}
Test(sf_memsuite, malloc_same_free_size, .init = sf_mem_init, .fini = sf_mem_fini) {
  int *x = sf_malloc(32);
  int *y = sf_malloc(48);
  int *z = sf_malloc(48);
  int *use_this = y;
  sf_free(y);
  int *newS = sf_malloc(48);
  *x=1;
  *z=1;
  info pt = {0,0,0,0,0,0};
  info *ptr = &pt;
  sf_info(ptr);
   cr_assert((unsigned long)use_this==(unsigned long)newS,"should use the same block");
   sf_header test = *((sf_header*)newS-1);
   uint64_t offset = (test.block_size<<4)-16;
   offset = offset/sizeof(sf_footer);
    cr_assert(test.block_size<<4==64,"should be 64");
     cr_assert(test.alloc==1,"should be 1");
     sf_footer test2 = *((sf_footer*)newS+offset);
     cr_assert(test2.alloc==1,"should be 1");
      cr_assert(test2.block_size<<4==64,"should be 64");
cr_assert(freelist_head->header.block_size<<4==3920,"should be 3920 block size");
cr_assert(freelist_head->next==NULL,"only one");


}
/* test only
*/
Test(sf_memsuite, Test1_Malloc_Size_400, .init = sf_mem_init, .fini = sf_mem_fini) {
  int *x = sf_malloc(400);
  sf_header *sfHeader = (sf_header *)((char*)x - 8);
  cr_assert(sfHeader->alloc == 1, "Header alloc is not 1!");
  cr_assert(sfHeader->requested_size == 400, "Header requested_size is not 400!");
  cr_assert(sfHeader->block_size == (416 >> 4), "Header block_size is not 416 >> 4!");

  sf_footer *sfFooter = (sf_footer *)(sfHeader + (sfHeader->block_size << 4)/8 - 1);
  cr_assert(sfFooter->alloc == 1, "Footer alloc is not 1!");
  cr_assert(sfFooter->block_size == (416 >> 4), "Footer block_size is not 416 >> 4!");
}

Test(sf_memsuite, Test2_Malloc_Size_4096, .init = sf_mem_init, .fini = sf_mem_fini) {
  int *x = sf_malloc(4096);
  cr_assert(x != NULL, "The ptr is NULL!");
  sf_header *sfHeader = (sf_header *)((char*)x - 8);
  cr_assert(sfHeader->alloc == 1, "Header alloc is not 1!");
  cr_assert(sfHeader->requested_size == 4096, "Header requested_size is not 4096!");
  cr_assert(sfHeader->block_size == (4112 >> 4), "Header block_size is not 4104 >> 4!");

  sf_footer *sfFooter = (sf_footer *)(sfHeader + (sfHeader->block_size << 4)/8 - 1);
  cr_assert(sfFooter->alloc == 1, "Footer alloc is not 1!");
  cr_assert(sfFooter->block_size == (4112 >> 4), "Footer block_size is not 4104 >> 4!");
}

Test(sf_memsuite, Test3_Malloc_Size_16368, .init = sf_mem_init, .fini = sf_mem_fini) {
  int *x = sf_malloc(16368);
  cr_assert(x != NULL, "The ptr is NULL!");
  sf_header *sfHeader = (sf_header *)((char*)x - 8);
  cr_assert(sfHeader->alloc == 1, "Header alloc is not 1!");
  cr_assert(sfHeader->requested_size == 16368, "Header requested_size is not 16368!");
  cr_assert(sfHeader->block_size == (16384>>4), "Header block_size is not 16384>>4!");

  sf_footer *sfFooter = (sf_footer *)(sfHeader + (sfHeader->block_size << 4)/8 - 1);
  cr_assert(sfFooter->alloc == 1, "Footer alloc is not 1!");
  cr_assert(sfFooter->block_size == (16384 >> 4), "Footer block_size is not 16384>>4!");
}

Test(sf_memsuite, Test4_Malloc_Size_16369, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *x = sf_malloc(16369);
  cr_assert(x == NULL, "The ptr is not NULL!");
  cr_assert(errno == ENOMEM, "ERRNO not set to ENOMEM");
}

Test(sf_memsuite, Test5_Malloc_Size_0, .init = sf_mem_init, .fini = sf_mem_fini) {
  int *x = sf_malloc(0);
  cr_assert(x == NULL, "The ptr is not NULL!");
  cr_assert(errno == EINVAL, "ERRNO not set to EINVAL");
}

Test(sf_memsuite, Test6_Padding_Size_0, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *x = sf_malloc(16);
  sf_header *sfHeader = (sf_header *)((char*)x - 8);
  cr_assert(sfHeader->alloc == 1, "Header Alloc is not 1");
  cr_assert(sfHeader->block_size == (32>>4), "Header block_size is not 32 >> 4");
  cr_assert(sfHeader->requested_size == 16, "requested_size is not 16");
  cr_assert(sfHeader->padding_size == 0, "padding_size is not 0");

  sf_footer *sfFooter = (sf_footer *)(sfHeader + (sfHeader->block_size << 4)/8 - 1);
  cr_assert(sfFooter->alloc == 1,"Footer alloc is not 1");
  cr_assert(sfFooter->block_size == (32>>4),"Footer block_size is not 32 >> 4");
}


Test(sf_memsuite, Test7_Padding_Size_1, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *x = sf_malloc(15);
  sf_header *sfHeader = (sf_header *)((char*)x - 8);
  cr_assert(sfHeader->alloc == 1, "Header Alloc is not 1");
  cr_assert(sfHeader->block_size == (32>>4), "Header block_size is not 32 >> 4");
  cr_assert(sfHeader->requested_size == 15, "requested_size is not 15");
  cr_assert(sfHeader->padding_size == 1, "padding_size is not 1");

  sf_footer *sfFooter = (sf_footer *)(sfHeader + (sfHeader->block_size << 4)/8 - 1);
  cr_assert(sfFooter->alloc == 1,"Footer alloc is not 1");
  cr_assert(sfFooter->block_size == (32>>4),"Footer block_size is not 32 >> 4");
}


Test(sf_memsuite, Test8_Padding_Size_2, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *x = sf_malloc(14);
  sf_header *sfHeader = (sf_header *)((char*)x - 8);
  cr_assert(sfHeader->alloc == 1, "Header Alloc is not 1");
  cr_assert(sfHeader->block_size == (32>>4), "Header block_size is not 32 >> 4");
  cr_assert(sfHeader->requested_size == 14, "requested_size is not 14");
  cr_assert(sfHeader->padding_size == 2, "padding_size is not 2");

  sf_footer *sfFooter = (sf_footer *)(sfHeader + (sfHeader->block_size << 4)/8 - 1);
  cr_assert(sfFooter->alloc == 1,"Footer alloc is not 1");
  cr_assert(sfFooter->block_size == (32>>4),"Footer block_size is not 32 >> 4");
}


Test(sf_memsuite, Test9_Padding_Size_3, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *x = sf_malloc(13);
  sf_header *sfHeader = (sf_header *)((char*)x - 8);
  cr_assert(sfHeader->alloc == 1, "Header Alloc is not 1");
  cr_assert(sfHeader->block_size == (32>>4), "Header block_size is not 32 >> 4");
  cr_assert(sfHeader->requested_size == 13, "requested_size is not 13");
  cr_assert(sfHeader->padding_size == 3, "padding_size is not 3");

  sf_footer *sfFooter = (sf_footer *)(sfHeader + (sfHeader->block_size << 4)/8 - 1);
  cr_assert(sfFooter->alloc == 1,"Footer alloc is not 1");
  cr_assert(sfFooter->block_size == (32>>4),"Footer block_size is not 32 >> 4");
}

Test(sf_memsuite, Test10_Padding_Size_4, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *x = sf_malloc(12);
  sf_header *sfHeader = (sf_header *)((char*)x - 8);
  cr_assert(sfHeader->alloc == 1, "Header Alloc is not 1");
  cr_assert(sfHeader->block_size == (32>>4), "Header block_size is not 32 >> 4");
  cr_assert(sfHeader->requested_size == 12, "requested_size is not 12");
  cr_assert(sfHeader->padding_size == 4, "padding_size is not 4");

  sf_footer *sfFooter = (sf_footer *)(sfHeader + (sfHeader->block_size << 4)/8 - 1);
  cr_assert(sfFooter->alloc == 1,"Footer alloc is not 1");
  cr_assert(sfFooter->block_size == (32>>4),"Footer block_size is not 32 >> 4");
}


Test(sf_memsuite, Test11_Padding_Size_5, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *x = sf_malloc(11);
  sf_header *sfHeader = (sf_header *)((char*)x - 8);
  cr_assert(sfHeader->alloc == 1, "Header Alloc is not 1");
  cr_assert(sfHeader->block_size == (32>>4), "Header block_size is not 32 >> 4");
  cr_assert(sfHeader->requested_size == 11, "requested_size is not 11");
  cr_assert(sfHeader->padding_size == 5, "padding_size is not 5");

  sf_footer *sfFooter = (sf_footer *)(sfHeader + (sfHeader->block_size << 4)/8 - 1);
  cr_assert(sfFooter->alloc == 1,"Footer alloc is not 1");
  cr_assert(sfFooter->block_size == (32>>4),"Footer block_size is not 32 >> 4");
}


Test(sf_memsuite, Test12_Padding_Size_6, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *x = sf_malloc(10);
  sf_header *sfHeader = (sf_header *)((char*)x - 8);
  cr_assert(sfHeader->alloc == 1, "Header Alloc is not 1");
  cr_assert(sfHeader->block_size == (32>>4), "Header block_size is not 32 >> 4");
  cr_assert(sfHeader->requested_size == 10, "requested_size is not 10");
  cr_assert(sfHeader->padding_size == 6, "padding_size is not 6");

  sf_footer *sfFooter = (sf_footer *)(sfHeader + (sfHeader->block_size << 4)/8 - 1);
  cr_assert(sfFooter->alloc == 1,"Footer alloc is not 1");
  cr_assert(sfFooter->block_size == (32>>4),"Footer block_size is not 32 >> 4");
}


Test(sf_memsuite, Test13_Padding_Size_7, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *x = sf_malloc(9);
  sf_header *sfHeader = (sf_header *)((char*)x - 8);
  cr_assert(sfHeader->alloc == 1, "Header Alloc is not 1");
  cr_assert(sfHeader->block_size == (32>>4), "Header block_size is not 32 >> 4");
  cr_assert(sfHeader->requested_size == 9, "requested_size is not 9");
  cr_assert(sfHeader->padding_size == 7, "padding_size is not 7");

  sf_footer *sfFooter = (sf_footer *)(sfHeader + (sfHeader->block_size << 4)/8 - 1);
  cr_assert(sfFooter->alloc == 1,"Footer alloc is not 1");
  cr_assert(sfFooter->block_size == (32>>4),"Footer block_size is not 32 >> 4");
}

Test(sf_memsuite, Test14_Padding_Size_8, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *x = sf_malloc(8);
  sf_header *sfHeader = (sf_header *)((char*)x - 8);
  cr_assert(sfHeader->alloc == 1, "Header Alloc is not 1");
  cr_assert(sfHeader->block_size == (32>>4), "Header block_size is not 32 >> 4");
  cr_assert(sfHeader->requested_size == 8, "requested_size is not 8");
  cr_assert(sfHeader->padding_size == 8, "padding_size is not 8");

  sf_footer *sfFooter = (sf_footer *)(sfHeader + (sfHeader->block_size << 4)/8 - 1);
  cr_assert(sfFooter->alloc == 1,"Footer alloc is not 1");
  cr_assert(sfFooter->block_size == (32>>4),"Footer block_size is not 32 >> 4");
}


Test(sf_memsuite, Test15_Padding_Size_9, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *x = sf_malloc(7);
  sf_header *sfHeader = (sf_header *)((char*)x - 8);
  cr_assert(sfHeader->alloc == 1, "Header Alloc is not 1");
  cr_assert(sfHeader->block_size == (32>>4), "Header block_size is not 32 >> 4");
  cr_assert(sfHeader->requested_size == 7, "requested_size is not 7");
  cr_assert(sfHeader->padding_size == 9, "padding_size is not 9");

  sf_footer *sfFooter = (sf_footer *)(sfHeader + (sfHeader->block_size << 4)/8 - 1);
  cr_assert(sfFooter->alloc == 1,"Footer alloc is not 1");
  cr_assert(sfFooter->block_size == (32>>4),"Footer block_size is not 32 >> 4");
}


Test(sf_memsuite, Test16_Padding_Size_10, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *x = sf_malloc(6);
  sf_header *sfHeader = (sf_header *)((char*)x - 8);
  cr_assert(sfHeader->alloc == 1, "Header Alloc is not 1");
  cr_assert(sfHeader->block_size == (32>>4), "Header block_size is not 32 >> 4");
  cr_assert(sfHeader->requested_size == 6, "requested_size is not 6");
  cr_assert(sfHeader->padding_size == 10, "padding_size is not 10");

  sf_footer *sfFooter = (sf_footer *)(sfHeader + (sfHeader->block_size << 4)/8 - 1);
  cr_assert(sfFooter->alloc == 1,"Footer alloc is not 1");
  cr_assert(sfFooter->block_size == (32>>4),"Footer block_size is not 32 >> 4");
}


Test(sf_memsuite, Test17_Padding_Size_11, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *x = sf_malloc(5);
  sf_header *sfHeader = (sf_header *)((char*)x - 8);
  cr_assert(sfHeader->alloc == 1, "Header Alloc is not 1");
  cr_assert(sfHeader->block_size == (32>>4), "Header block_size is not 32 >> 4");
  cr_assert(sfHeader->requested_size == 5, "requested_size is not 5");
  cr_assert(sfHeader->padding_size == 11, "padding_size is not 11");

  sf_footer *sfFooter = (sf_footer *)(sfHeader + (sfHeader->block_size << 4)/8 - 1);
  cr_assert(sfFooter->alloc == 1,"Footer alloc is not 1");
  cr_assert(sfFooter->block_size == (32>>4),"Footer block_size is not 32 >> 4");
}

Test(sf_memsuite, Test18_Padding_Size_12, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *x = sf_malloc(4);
  sf_header *sfHeader = (sf_header *)((char*)x - 8);
  cr_assert(sfHeader->alloc == 1, "Header Alloc is not 1");
  cr_assert(sfHeader->block_size == (32>>4), "Header block_size is not 32 >> 4");
  cr_assert(sfHeader->requested_size == 4, "requested_size is not 4");
  cr_assert(sfHeader->padding_size == 12, "padding_size is not 12");

  sf_footer *sfFooter = (sf_footer *)(sfHeader + (sfHeader->block_size << 4)/8 - 1);
  cr_assert(sfFooter->alloc == 1,"Footer alloc is not 1");
  cr_assert(sfFooter->block_size == (32>>4),"Footer block_size is not 32 >> 4");
}


Test(sf_memsuite, Test19_Padding_Size_13, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *x = sf_malloc(3);
  sf_header *sfHeader = (sf_header *)((char*)x - 8);
  cr_assert(sfHeader->alloc == 1, "Header Alloc is not 1");
  cr_assert(sfHeader->block_size == (32>>4), "Header block_size is not 32 >> 4");
  cr_assert(sfHeader->requested_size == 3, "requested_size is not 3");
  cr_assert(sfHeader->padding_size == 13, "padding_size is not 13");

  sf_footer *sfFooter = (sf_footer *)(sfHeader + (sfHeader->block_size << 4)/8 - 1);
  cr_assert(sfFooter->alloc == 1,"Footer alloc is not 1");
  cr_assert(sfFooter->block_size == (32>>4),"Footer block_size is not 32 >> 4");
}


Test(sf_memsuite, Test20_Padding_Size_14, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *x = sf_malloc(2);
  sf_header *sfHeader = (sf_header *)((char*)x - 8);
  cr_assert(sfHeader->alloc == 1, "Header Alloc is not 1");
  cr_assert(sfHeader->block_size == (32>>4), "Header block_size is not 32 >> 4");
  cr_assert(sfHeader->requested_size == 2, "requested_size is not 2");
  cr_assert(sfHeader->padding_size == 14, "padding_size is not 14");

  sf_footer *sfFooter = (sf_footer *)(sfHeader + (sfHeader->block_size << 4)/8 - 1);
  cr_assert(sfFooter->alloc == 1,"Footer alloc is not 1");
  cr_assert(sfFooter->block_size == (32>>4),"Footer block_size is not 32 >> 4");
}


Test(sf_memsuite, Test21_Padding_Size_15, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *x = sf_malloc(1);
  sf_header *sfHeader = (sf_header *)((char*)x - 8);
  cr_assert(sfHeader->alloc == 1, "Header Alloc is not 1");
  cr_assert(sfHeader->block_size == (32>>4), "Header block_size is not 32 >> 4");
  cr_assert(sfHeader->requested_size == 1, "requested_size is not 1");
  cr_assert(sfHeader->padding_size == 15, "padding_size is not 15");

  sf_footer *sfFooter = (sf_footer *)(sfHeader + (sfHeader->block_size << 4)/8 - 1);
  cr_assert(sfFooter->alloc == 1,"Footer alloc is not 1");
  cr_assert(sfFooter->block_size == (32>>4),"Footer block_size is not 32 >> 4");
}

Test(sf_memsuite, Test22_Find_Best_Fit_1, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(1);
  char *b = sf_malloc(1);
  char *c = sf_malloc(1);
  char *d = sf_malloc(1);
  char *e = sf_malloc(1);
  sf_free(b);
  sf_free(d);
  char *f = sf_malloc(1);
  cr_assert(f == b, "Did not find best place");
  sf_free(a);
  sf_free(c);
  sf_free(e);
}

Test(sf_memsuite, Test23_Find_Best_Fit_2, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(1);
  char *b = sf_malloc(20);
  char *c = sf_malloc(1);
  char *d = sf_malloc(1);
  char *e = sf_malloc(1);
  sf_free(b);
  sf_free(d);
  char *f = sf_malloc(1);
  cr_assert(f == d, "Did not find best place");
  sf_free(a);
  sf_free(c);
  sf_free(e);
}

Test(sf_memsuite, Test24_Find_Best_Fit_3, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(1);
  char *b = sf_malloc(1);
  char *c = sf_malloc(1);
  char *d = sf_malloc(20);
  char *e = sf_malloc(1);
  sf_free(b);
  sf_free(d);
  char *f = sf_malloc(1);
  cr_assert(f == b, "Did not find best place");
  sf_free(a);
  sf_free(c);
  sf_free(e);
}

Test(sf_memsuite, Test25_Free_List_Header_Block_Size_1, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(1);
  cr_assert((freelist_head->header).block_size == 0x100 - 2, "Before free Header block_size is %d", (freelist_head->header).block_size);
  sf_footer* freelist_tail = (sf_footer*)((unsigned long int)(freelist_head) + ((freelist_head->header).block_size << 4) - 8);
  cr_assert(freelist_tail->block_size == 0x100 - 2, "Before free Footer block_size is not correct");

  sf_free(a);
  cr_assert((freelist_head->header).block_size == 0x100, "Header block_size is %d", (freelist_head->header).block_size);
  freelist_tail = (sf_footer*)((unsigned long int)(freelist_head) + ((freelist_head->header).block_size << 4) - 8);
  cr_assert(freelist_tail->block_size == 0x100, "Footer block_size is not correct");
}

Test(sf_memsuite, Test26_Free_List_Header_Block_Size_2, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(4096);
  cr_assert((freelist_head->header).block_size == 0x200 - 0x100 - 1, "Before free Header block_size is %d", (freelist_head->header).block_size);
  sf_footer* freelist_tail = (sf_footer*)((unsigned long int)(freelist_head) + ((freelist_head->header).block_size << 4) - 8);
  cr_assert(freelist_tail->block_size == 0x200 - 0x100 - 1, "Before free Footer block_size is not correct");

  sf_free(a);
  cr_assert((freelist_head->header).block_size == 0x200, "Header block_size is %d", (freelist_head->header).block_size);
  freelist_tail = (sf_footer*)((unsigned long int)(freelist_head) + ((freelist_head->header).block_size << 4) - 8);
  cr_assert(freelist_tail->block_size == 0x200, "Footer block_size is not correct");
}

Test(sf_memsuite, Test27_Free_List_Header_Block_Size_3, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(16368);
  cr_assert(freelist_head == NULL, "freelist_head is not null");

  sf_free(a);
  cr_assert((freelist_head->header).block_size == 0x400, "Header block_size is %d", (freelist_head->header).block_size);
  sf_footer* freelist_tail = (sf_footer*)((unsigned long int)(freelist_head) + ((freelist_head->header).block_size << 4) - 8);
  cr_assert(freelist_tail->block_size == 0x400, "Footer block_size is not correct");
}

Test(sf_memsuite, Test28_NULL_Free_List_Header, .init = sf_mem_init, .fini = sf_mem_fini) {
  cr_assert(freelist_head == NULL, "freelist_head is not null");
}

Test(sf_memsuite, Test29_Coalescing_Check_1, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(1);
  char *b = sf_malloc(1);
  char *c = sf_malloc(1);
  sf_free(b);
  sf_footer* freelist_tail = (sf_footer*)((unsigned long int)(freelist_head) + ((freelist_head->header).block_size << 4) - 8);
  cr_assert((freelist_head->header).block_size == 2, "Header block_size is %d", (freelist_head->header).block_size);
  cr_assert(freelist_tail->block_size == 2, "Footer block_size is not correct");
  sf_free(a);
  sf_free(c);
}

Test(sf_memsuite, Test30_Coalescing_Check_2, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(1);
  char *b = sf_malloc(1);
  char *c = sf_malloc(1);
  sf_free(b);
  sf_free(a);
  sf_footer* freelist_tail = (sf_footer*)((unsigned long int)(freelist_head) + ((freelist_head->header).block_size << 4) - 8);
  cr_assert((freelist_head->header).block_size == 4, "Header block_size is %d", (freelist_head->header).block_size);
  cr_assert(freelist_tail->block_size == 4, "Footer block_size is not correct");
  sf_free(c);
}

Test(sf_memsuite, Test31_Coalescing_Check_3, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(1);
  char *b = sf_malloc(1);
  char *c = sf_malloc(1);
  sf_free(a);
  sf_free(b);
  sf_footer* freelist_tail = (sf_footer*)((unsigned long int)(freelist_head) + ((freelist_head->header).block_size << 4) - 8);
  cr_assert((freelist_head->header).block_size == 4, "Header block_size is %d", (freelist_head->header).block_size);
  cr_assert(freelist_tail->block_size == 4, "Footer block_size is not correct");
  sf_free(c);
}

Test(sf_memsuite, Test32_Coalescing_Check_4, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(1);
  char *b = sf_malloc(1);
  char *c = sf_malloc(1);
  char *d = sf_malloc(1);
  sf_free(a);
  sf_free(c);
  sf_free(b);
  sf_footer* freelist_tail = (sf_footer*)((unsigned long int)(freelist_head) + ((freelist_head->header).block_size << 4) - 8);
  cr_assert((freelist_head->header).block_size == 6, "Header block_size is %d", (freelist_head->header).block_size);
  cr_assert(freelist_tail->block_size == 6, "Footer block_size is not correct");
  sf_free(d);
}

Test(sf_memsuite, Test33_Invalid_Free, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(1);
  sf_free(a);
  sf_free(a);
  cr_assert(errno == EINVAL, "errno did not set to EINVAL");
}

Test(sf_memsuite, Test34_info_allocatedBlocks, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(1);
  char *b = sf_malloc(2);
  info INFORM;
  info* infoPtr = &INFORM;
  sf_info(infoPtr);
  cr_assert(infoPtr->allocatedBlocks == 2, "Block number is not 2");
  char *c = sf_malloc(5000);
  sf_info(infoPtr);
  cr_assert(infoPtr->allocatedBlocks == 3, "Block number is not 3");
  sf_free(a);
  sf_free(b);
  sf_info(infoPtr);
  cr_assert(infoPtr->allocatedBlocks == 1, "Block number is not 1");
  sf_free(c);
  sf_info(infoPtr);
  cr_assert(infoPtr->allocatedBlocks == 0, "Block number is not 0");
}

Test(sf_memsuite, Test35_info_padding, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(1);
  char *b = sf_malloc(2);
  info INFORM;
  info* infoPtr = &INFORM;
  sf_info(infoPtr);
  cr_assert(infoPtr->padding == 29, "Padding in bytes is not 29");
  char *c = sf_malloc(5001);
  sf_info(infoPtr);
  cr_assert(infoPtr->padding == 36, "Padding in bytes is not 36");
  sf_free(a);
  sf_free(b);
  sf_info(infoPtr);
  cr_assert(infoPtr->padding == 7, "Padding in bytes is not 7");
  sf_free(c);
  sf_info(infoPtr);
  cr_assert(infoPtr->padding == 0, "Padding in bytes is not 0");
  char *d = sf_malloc(16);
  sf_info(infoPtr);
  cr_assert(infoPtr->padding == 0, "Padding in bytes is not 0");
  sf_free(d);
}

Test(sf_memsuite, Test36_info_coalesing, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(1);
  char *b = sf_malloc(2);
  info INFORM;
  info* infoPtr = &INFORM;
  sf_info(infoPtr);
  cr_assert(infoPtr->coalesces == 0, "coalesces is not 0");
  char *c = sf_malloc(5001);
  sf_info(infoPtr);
  cr_assert(infoPtr->coalesces == 1, "coalesces is not 1");
  sf_free(a);
  sf_free(b);
  sf_info(infoPtr);
  cr_assert(infoPtr->coalesces == 2, "coalesces is not 2");
  sf_free(c);
  sf_info(infoPtr);
  cr_assert(infoPtr->coalesces == 3, "coalesces is not 3");
  char *d = sf_malloc(16);
  sf_info(infoPtr);
  cr_assert(infoPtr->coalesces == 3, "coalesces is not 3 again");
  sf_free(d);
  sf_info(infoPtr);
  cr_assert(infoPtr->coalesces == 4, "coalesces is not 4");
}


Test(sf_memsuite, Test37_info_another_coalesing, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(4080);
  info INFORM;
  info* infoPtr = &INFORM;
  sf_info(infoPtr);
  cr_assert(infoPtr->coalesces == 0, "coalesces is not 0");
  char *b = sf_malloc(5001);
  sf_info(infoPtr);
  cr_assert(infoPtr->coalesces == 0, "coalesces is not 0 again");
  sf_free(a);
  sf_free(b);
  sf_info(infoPtr);
  cr_assert(infoPtr->coalesces == 1, "coalesces is not 1");
  char *d = sf_malloc(16);
  sf_info(infoPtr);
  cr_assert(infoPtr->coalesces == 1, "coalesces is not 1 again");
  sf_free(d);
  sf_info(infoPtr);
  cr_assert(infoPtr->coalesces == 2, "coalesces is not 2");
}

Test(sf_memsuite, Test38_info_splinterBlocks, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(4060);
  info INFORM;
  info* infoPtr = &INFORM;
  sf_info(infoPtr);
  cr_assert(infoPtr->splinterBlocks == 1, "splinterBlocks is not 1");
  sf_free(a);
  sf_info(infoPtr);
  cr_assert(infoPtr->splinterBlocks == 0, "splinterBlocks is not 0");
  char *b = sf_malloc(32);
  char *c = sf_malloc(32);
  sf_free(b);
  char *d = sf_malloc(16);
  sf_info(infoPtr);
  cr_assert(infoPtr->splinterBlocks == 1, "splinterBlocks is not 1 again");
  sf_free(c);
  sf_free(d);
  sf_info(infoPtr);
  cr_assert(infoPtr->splinterBlocks == 0, "splinterBlocks is not 0 again");
}

Test(sf_memsuite, Test39_info_splintering, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(4060);
  info INFORM;
  info* infoPtr = &INFORM;
  sf_info(infoPtr);
  cr_assert(infoPtr->splintering == 16, "splintering is not 16");
  sf_free(a);
  sf_info(infoPtr);
  cr_assert(infoPtr->splintering == 0, "splintering is not 0");
  char *b = sf_malloc(32);
  char *c = sf_malloc(32);
  sf_free(b);
  char *d = sf_malloc(16);
  sf_info(infoPtr);
  cr_assert(infoPtr->splintering == 16, "splintering is not 16 again");
  sf_free(c);
  sf_free(d);
  sf_info(infoPtr);
  cr_assert(infoPtr->splintering == 0, "splintering is not 0 again");
}

Test(sf_memsuite, Test40_realloc_simple_block_1, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(32);
  char *b = sf_realloc(a, 16);
  cr_assert(a == b, "Ptr are not at the same position.");
  sf_header *sfHeader = (sf_header *)((char*)b - 8);
  cr_assert(sfHeader->alloc == 1, "Header alloc is not 1!");
  cr_assert(sfHeader->requested_size == 16, "Header requested_size is not 16!");
  cr_assert(sfHeader->block_size == (32 >> 4), "Header block_size is not 32 >> 4!");

  sf_footer *sfFooter = (sf_footer *)(sfHeader + (sfHeader->block_size << 4)/8 - 1);
  cr_assert(sfFooter->alloc == 1, "Footer alloc is not 1!");
  cr_assert(sfFooter->block_size == (32 >> 4), "Footer block_size is not 32 >> 4!");
  cr_assert(freelist_head != NULL, "freelist_head is NULL");
  cr_assert(((freelist_head->header).block_size) << 4 == 4064, "Free block size is not 4064, is %d", ((freelist_head->header).block_size) << 4);
  cr_assert(freelist_head->prev == NULL, "freelist_head prev is not NULL.");
  cr_assert(freelist_head->next == NULL, "freelist_head next is not NULL.");
}

Test(sf_memsuite, Test41_realloc_simple_block_2, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(32);
  char *b = sf_realloc(a, 48);
  cr_assert(a == b, "Ptr are not at the same position.");
  sf_header *sfHeader = (sf_header *)((char*)b - 8);
  cr_assert(sfHeader->alloc == 1, "Header alloc is not 1!");
  cr_assert(sfHeader->requested_size == 48, "Header requested_size is not 48!");
  cr_assert(sfHeader->block_size == (64 >> 4), "Header block_size is not 64 >> 4!");

  sf_footer *sfFooter = (sf_footer *)(sfHeader + (sfHeader->block_size << 4)/8 - 1);
  cr_assert(sfFooter->alloc == 1, "Footer alloc is not 1!");
  cr_assert(sfFooter->block_size == (64 >> 4), "Footer block_size is not 64 >> 4!");
  cr_assert(freelist_head != NULL, "freelist_head is NULL");
  cr_assert(((freelist_head->header).block_size) << 4 == 4032, "Free block size is not 4032");
  cr_assert(freelist_head->prev == NULL, "freelist_head prev is not NULL.");
  cr_assert(freelist_head->next == NULL, "freelist_head next is not NULL.");
}

Test(sf_memsuite, Test42_realloc_block_more_than_1_page, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(32);
  char *b = sf_realloc(a, 4096);
  cr_assert(a == b, "Ptr are not at the same position.");
  sf_header *sfHeader = (sf_header *)((char*)b - 8);
  cr_assert(sfHeader->alloc == 1, "Header alloc is not 1!");
  cr_assert(sfHeader->requested_size == 4096, "Header requested_size is not 4096!");
  cr_assert(sfHeader->block_size == (4112 >> 4), "Header block_size is not 4112 >> 4!");

  sf_footer *sfFooter = (sf_footer *)(sfHeader + (sfHeader->block_size << 4)/8 - 1);
  cr_assert(sfFooter->alloc == 1, "Footer alloc is not 1!");
  cr_assert(sfFooter->block_size == (4112 >> 4), "Footer block_size is not 4112 >> 4!");
}

Test(sf_memsuite, Test43_realloc_full_page, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(32);
  char *b = sf_realloc(a, 4080);
  cr_assert(a == b, "Ptr are not at the same position.");
  sf_header *sfHeader = (sf_header *)((char*)b - 8);
  cr_assert(sfHeader->alloc == 1, "Header alloc is not 1!");
  cr_assert(sfHeader->requested_size == 4080, "Header requested_size is not 4080!");
  cr_assert(sfHeader->block_size == (4096 >> 4), "Header block_size is not 4096 >> 4!");

  sf_footer *sfFooter = (sf_footer *)(sfHeader + (sfHeader->block_size << 4)/8 - 1);
  cr_assert(sfFooter->alloc == 1, "Footer alloc is not 1!");
  cr_assert(sfFooter->block_size == (4096 >> 4), "Footer block_size is not 4096 >> 4!");
  cr_assert(freelist_head == NULL, "freelist_head is not NULL");
}

Test(sf_memsuite, Test44_realloc_full_page_to_smaller, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(4080);
  char *b = sf_realloc(a, 32);
  cr_assert(a == b, "Ptr are not at the same position.");
  sf_header *sfHeader = (sf_header *)((char*)b - 8);
  cr_assert(sfHeader->alloc == 1, "Header alloc is not 1!");
  cr_assert(sfHeader->requested_size == 32, "Header requested_size is not 32!");
  cr_assert(sfHeader->block_size == (48 >> 4), "Header block_size is not 48 >> 4!");

  sf_footer *sfFooter = (sf_footer *)(sfHeader + (sfHeader->block_size << 4)/8 - 1);
  cr_assert(sfFooter->alloc == 1, "Footer alloc is not 1!");
  cr_assert(sfFooter->block_size == (48 >> 4), "Footer block_size is not 48 >> 4!");
  cr_assert(freelist_head != NULL, "freelist_head is NULL");
  cr_assert(freelist_head->header.block_size == 4048 >> 4, "freelist_head block_size is not 4048 >> 4");
}

Test(sf_memsuite, Test45_realloc_full_page_to_bigger, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(4080);
  char *b = sf_realloc(a, 4096);
  cr_assert(a == b, "Ptr are not at the same position.");
  sf_header *sfHeader = (sf_header *)((char*)b - 8);
  cr_assert(sfHeader->alloc == 1, "Header alloc is not 1!");
  cr_assert(sfHeader->requested_size == 4096, "Header requested_size is not 4096!");
  cr_assert(sfHeader->block_size == (4112 >> 4), "Header block_size is not 4112 >> 4!");

  sf_footer *sfFooter = (sf_footer *)(sfHeader + (sfHeader->block_size << 4)/8 - 1);
  cr_assert(sfFooter->alloc == 1, "Footer alloc is not 1!");
  cr_assert(sfFooter->block_size == (4112 >> 4), "Footer block_size is not 4112 >> 4!");
  cr_assert(freelist_head != NULL, "freelist_head is NULL");
  cr_assert(freelist_head->header.block_size == 4080 >> 4, "freelist_head block_size is not 4080 >> 4");
}

Test(sf_memsuite, Test46_realloc_0, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(4080);
  char *b = sf_realloc(a, 0);
  cr_assert(b == NULL, "Ptr is not NULL.");
  cr_assert(freelist_head != NULL, "freelist_head is NULL");
  cr_assert(freelist_head->next == NULL, "freelist_head->next != NULL");
  cr_assert(freelist_head->prev == NULL, "freelist_head->prev != NULL");
  cr_assert(freelist_head == (void*)(a - 8), "Address is not correct.");
  cr_assert(freelist_head->header.alloc == 0, "freelist_head alloc is not 0");
  cr_assert(freelist_head->header.splinter == 0, "freelist_head splinter is not 0");
  cr_assert(freelist_head->header.block_size == 4096 >> 4, "freelist_head block_size is not 4096 >> 4");
  sf_footer* freelist_head_footer = (sf_footer*)((char*) freelist_head + ((freelist_head->header.block_size) << 4) - 8);
  cr_assert(freelist_head_footer->alloc == 0, "freelist_head footer alloc is not 0");
  cr_assert(freelist_head_footer->splinter == 0, "freelist_head footer splinter is not 0");
  cr_assert(freelist_head_footer->block_size == 4096 >> 4, "freelist_head footer block_size is not 4096 >> 4");
  info INFORM;
  info* infoPtr = &INFORM;
  sf_info(infoPtr);
  cr_assert(infoPtr->allocatedBlocks == 0, "allocatedBlocks is not 0");
  cr_assert(infoPtr->splinterBlocks == 0, "splinterBlocks is not 0");
  cr_assert(infoPtr->padding == 0, "padding is not 0");
  cr_assert(infoPtr->splintering == 0, "splintering is not 0");
  cr_assert(infoPtr->coalesces == 0, "coalesces is not 0");
  cr_assert(infoPtr->peakMemoryUtilization == 4080.0/4096.0, "peakMemoryUtilization is not 4080/4096");
}

Test(sf_memsuite, Test47_realloc_0, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(32);
  char *b = sf_realloc(a, 0);
  cr_assert(b == NULL, "Ptr is not NULL.");
  cr_assert(freelist_head != NULL, "freelist_head is NULL");
  cr_assert(freelist_head->next == NULL, "freelist_head->next != NULL");
  cr_assert(freelist_head->prev == NULL, "freelist_head->prev != NULL");
  cr_assert(freelist_head == (void*)(a - 8), "Address is not correct.");
  cr_assert(freelist_head->header.alloc == 0, "freelist_head alloc is not 0");
  cr_assert(freelist_head->header.splinter == 0, "freelist_head splinter is not 0");
  cr_assert(freelist_head->header.block_size == 4096 >> 4, "freelist_head block_size is not 4096 >> 4");
  sf_footer* freelist_head_footer = (sf_footer*)((char*) freelist_head + ((freelist_head->header.block_size) << 4) - 8);
  cr_assert(freelist_head_footer->alloc == 0, "freelist_head footer alloc is not 0");
  cr_assert(freelist_head_footer->splinter == 0, "freelist_head footer splinter is not 0");
  cr_assert(freelist_head_footer->block_size == 4096 >> 4, "freelist_head footer block_size is not 4096 >> 4");
  info INFORM;
  info* infoPtr = &INFORM;
  sf_info(infoPtr);
  cr_assert(infoPtr->allocatedBlocks == 0, "allocatedBlocks is not 0");
  cr_assert(infoPtr->splinterBlocks == 0, "splinterBlocks is not 0");
  cr_assert(infoPtr->padding == 0, "padding is not 0");
  cr_assert(infoPtr->splintering == 0, "splintering is not 0");
  cr_assert(infoPtr->coalesces == 1, "coalesces is not 1");
  cr_assert(infoPtr->peakMemoryUtilization == 32.0/4096.0, "peakMemoryUtilization is not 32/4096");
}

Test(sf_memsuite, Test48_realloc_invalid, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(4080);
  char *b = sf_realloc(a, 20000);
  cr_assert(b == NULL, "Ptr is not NULL.");
  cr_assert(errno == ENOMEM, "errno is not ENOMEM");
}

Test(sf_memsuite, Test49_realloc_same_block_1, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(15);
  *a = 100;
  char *b = sf_realloc(a, 13);
  cr_assert(b == a, "b != a");
  cr_assert(*b == 100, "Did not sign value");
  cr_assert(freelist_head != NULL, "freelist_head is NULL");
  cr_assert(freelist_head->next == NULL, "freelist_head->next != NULL");
  cr_assert(freelist_head->prev == NULL, "freelist_head->prev != NULL");
  cr_assert(freelist_head == (void*)(a + 24), "Address is not correct.");
  cr_assert(freelist_head->header.alloc == 0, "freelist_head alloc is not 0");
  cr_assert(freelist_head->header.splinter == 0, "freelist_head splinter is not 0");
  cr_assert(freelist_head->header.block_size == 4064 >> 4, "freelist_head block_size is not 4064 >> 4");
  sf_footer* freelist_head_footer = (sf_footer*)((char*) freelist_head + ((freelist_head->header.block_size) << 4) - 8);
  cr_assert(freelist_head_footer->alloc == 0, "freelist_head footer alloc is not 0");
  cr_assert(freelist_head_footer->splinter == 0, "freelist_head footer splinter is not 0");
  cr_assert(freelist_head_footer->block_size == 4064 >> 4, "freelist_head footer block_size is not 4064 >> 4");
  sf_header* bHeader = (sf_header*)(b - 8);
  cr_assert(bHeader->alloc == 1, "bHeader alloc is not 1");
  cr_assert(bHeader->splinter == 0, "bHeader splinter is not 0");
  cr_assert(bHeader->block_size == 32 >> 4, "bHeader block_size is not 32 >> 4");
  cr_assert(bHeader->requested_size == 13, "bHeader requested_size is not 13");
  cr_assert(bHeader->splinter_size == 0, "bHeader splinter_size is not 0");
  cr_assert(bHeader->padding_size == 3, "bHeader padding_size is not 3");
  sf_footer* bFooter = (sf_footer*)(b + (bHeader->block_size << 4) - 16);
  cr_assert(bFooter->alloc == 1, "bFooter alloc is not 1");
  cr_assert(bFooter->splinter == 0, "bFooter splinter is not 0");
  cr_assert(bFooter->block_size == 32 >> 4, "bFooter block_size is not 32 >> 4");
  info INFORM;
  info* infoPtr = &INFORM;
  sf_info(infoPtr);
  cr_assert(infoPtr->allocatedBlocks == 1, "allocatedBlocks is not 1");
  cr_assert(infoPtr->splinterBlocks == 0, "splinterBlocks is not 0");
  cr_assert(infoPtr->padding == 3, "padding is not 3");
  cr_assert(infoPtr->splintering == 0, "splintering is not 0");
  cr_assert(infoPtr->coalesces == 0, "coalesces is not 0");
  cr_assert(infoPtr->peakMemoryUtilization == 15.0/4096.0, "peakMemoryUtilization is not 15/4096");
}

Test(sf_memsuite, Test50_realloc_same_block_1_remove_splinter, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(17);
  char *c = sf_malloc(16);
  sf_free(a);
  a = sf_malloc(15);
  *a = 60;
  sf_free(c);
  char *b = sf_realloc(a, 13);
  cr_assert(b == a, "b != a");
  cr_assert(*b == 60, "did not sign the value");
  cr_assert(freelist_head != NULL, "freelist_head is NULL");
  cr_assert(freelist_head->next == NULL, "freelist_head->next != NULL");
  cr_assert(freelist_head->prev == NULL, "freelist_head->prev != NULL");
  cr_assert(freelist_head == (void*)(a + 32 - 8), "Address is not correct.");
  cr_assert(freelist_head->header.alloc == 0, "freelist_head alloc is not 0");
  cr_assert(freelist_head->header.splinter == 0, "freelist_head splinter is not 0");
  cr_assert(freelist_head->header.block_size == 4064 >> 4, "freelist_head block_size is not 4064 >> 4");
  sf_footer* freelist_head_footer = (sf_footer*)((char*) freelist_head + ((freelist_head->header.block_size) << 4) - 8);
  cr_assert(freelist_head_footer->alloc == 0, "freelist_head footer alloc is not 0");
  cr_assert(freelist_head_footer->splinter == 0, "freelist_head footer splinter is not 0");
  cr_assert(freelist_head_footer->block_size == 4064 >> 4, "freelist_head footer block_size is not 4064 >> 4");
  sf_header* bHeader = (sf_header*)(b - 8);
  cr_assert(bHeader->alloc == 1, "bHeader alloc is not 1");
  cr_assert(bHeader->splinter == 0, "bHeader splinter is not 0");
  cr_assert(bHeader->block_size == 32 >> 4, "bHeader block_size is not 32 >> 4");
  cr_assert(bHeader->requested_size == 13, "bHeader requested_size is not 13");
  cr_assert(bHeader->splinter_size == 0, "bHeader splinter_size is not 0");
  cr_assert(bHeader->padding_size == 3, "bHeader padding_size is not 3");
  sf_footer* bFooter = (sf_footer*)(b + (bHeader->block_size << 4) - 16);
  cr_assert(bFooter->alloc == 1, "bFooter alloc is not 1");
  cr_assert(bFooter->splinter == 0, "bFooter splinter is not 0");
  cr_assert(bFooter->block_size == 32 >> 4, "bFooter block_size is not 32 >> 4");
  info INFORM;
  info* infoPtr = &INFORM;
  sf_info(infoPtr);
  cr_assert(infoPtr->allocatedBlocks == 1, "allocatedBlocks is not 1");
  cr_assert(infoPtr->splinterBlocks == 0, "splinterBlocks is not 0");
  cr_assert(infoPtr->padding == 3, "padding is not 3");
  cr_assert(infoPtr->splintering == 0, "splintering is not 0");
  cr_assert(infoPtr->coalesces == 2, "coalesces is not 2");
  cr_assert(infoPtr->peakMemoryUtilization == 33.0/4096.0, "peakMemoryUtilization is not 33/4096");
  sf_free(c);
}


Test(sf_memsuite, Test51_realloc_same_block_2, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(17);
  char *c = sf_malloc(16);
  *a = 20;
  char *b = sf_realloc(a, 15);
  cr_assert(b == a, "b != a");
  cr_assert(*b == 20, "did not sign value");
  cr_assert(freelist_head != NULL, "freelist_head is NULL");
  cr_assert(freelist_head->next == NULL, "freelist_head->next != NULL");
  cr_assert(freelist_head->prev == NULL, "freelist_head->prev != NULL");
  cr_assert(freelist_head == (void*)(a + 80 - 8), "Address is not correct.");
  cr_assert(freelist_head->header.alloc == 0, "freelist_head alloc is not 0");
  cr_assert(freelist_head->header.splinter == 0, "freelist_head splinter is not 0");
  cr_assert(freelist_head->header.block_size == 4016 >> 4, "freelist_head block_size is not 4016 >> 4");
  sf_footer* freelist_head_footer = (sf_footer*)((char*) freelist_head + ((freelist_head->header.block_size) << 4) - 8);
  cr_assert(freelist_head_footer->alloc == 0, "freelist_head footer alloc is not 0");
  cr_assert(freelist_head_footer->splinter == 0, "freelist_head footer splinter is not 0");
  cr_assert(freelist_head_footer->block_size == 4016 >> 4, "freelist_head footer block_size is not 4016 >> 4");
  sf_header* bHeader = (sf_header*)(b - 8);
  cr_assert(bHeader->alloc == 1, "bHeader alloc is not 1");
  cr_assert(bHeader->splinter == 1, "bHeader splinter is not 1");
  cr_assert(bHeader->block_size == 48 >> 4, "bHeader block_size is not 48 >> 4");
  cr_assert(bHeader->requested_size == 15, "bHeader requested_size is not 15");
  cr_assert(bHeader->splinter_size == 16, "bHeader splinter_size is not 16");
  cr_assert(bHeader->padding_size == 1, "bHeader padding_size is not 1");
  sf_footer* bFooter = (sf_footer*)(b + (bHeader->block_size << 4) - 16);
  cr_assert(bFooter->alloc == 1, "bFooter alloc is not 1");
  cr_assert(bFooter->splinter == 1, "bFooter splinter is not 1");
  cr_assert(bFooter->block_size == 48 >> 4, "bFooter block_size is not 48 >> 4");
  info INFORM;
  info* infoPtr = &INFORM;
  sf_info(infoPtr);
  cr_assert(infoPtr->allocatedBlocks == 2, "allocatedBlocks is not 2");
  cr_assert(infoPtr->splinterBlocks == 1, "splinterBlocks is not 1");
  cr_assert(infoPtr->padding == 1, "padding is not 1");
  cr_assert(infoPtr->splintering == 16, "splintering is not 16");
  cr_assert(infoPtr->coalesces == 0, "coalesces is not 0");
  cr_assert(infoPtr->peakMemoryUtilization == 33.0/4096.0, "peakMemoryUtilization is not 33/4096");
  sf_free(c);
}

Test(sf_memsuite, Test52_realloc_same_block_3, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(15);
  *a = 28;
  char *b = sf_realloc(a, 15);
  cr_assert(b == a, "b != a");
  cr_assert(*b == 28, "did not sign value");
  cr_assert(freelist_head != NULL, "freelist_head is NULL");
  cr_assert(freelist_head->next == NULL, "freelist_head->next != NULL");
  cr_assert(freelist_head->prev == NULL, "freelist_head->prev != NULL");
  cr_assert(freelist_head == (void*)(a + 24), "Address is not correct.");
  cr_assert(freelist_head->header.alloc == 0, "freelist_head alloc is not 0");
  cr_assert(freelist_head->header.splinter == 0, "freelist_head splinter is not 0");
  cr_assert(freelist_head->header.block_size == 4064 >> 4, "freelist_head block_size is not 4064 >> 4");
  sf_footer* freelist_head_footer = (sf_footer*)((char*) freelist_head + ((freelist_head->header.block_size) << 4) - 8);
  cr_assert(freelist_head_footer->alloc == 0, "freelist_head footer alloc is not 0");
  cr_assert(freelist_head_footer->splinter == 0, "freelist_head footer splinter is not 0");
  cr_assert(freelist_head_footer->block_size == 4064 >> 4, "freelist_head footer block_size is not 4064 >> 4");
  sf_header* bHeader = (sf_header*)(b - 8);
  cr_assert(bHeader->alloc == 1, "bHeader alloc is not 1");
  cr_assert(bHeader->splinter == 0, "bHeader splinter is not 0");
  cr_assert(bHeader->block_size == 32 >> 4, "bHeader block_size is not 32 >> 4");
  cr_assert(bHeader->requested_size == 15, "bHeader requested_size is not 15");
  cr_assert(bHeader->splinter_size == 0, "bHeader splinter_size is not 0");
  cr_assert(bHeader->padding_size == 1, "bHeader padding_size is not 1");
  sf_footer* bFooter = (sf_footer*)(b + (bHeader->block_size << 4) - 16);
  cr_assert(bFooter->alloc == 1, "bFooter alloc is not 1");
  cr_assert(bFooter->splinter == 0, "bFooter splinter is not 0");
  cr_assert(bFooter->block_size == 32 >> 4, "bFooter block_size is not 32 >> 4");
  info INFORM;
  info* infoPtr = &INFORM;
  sf_info(infoPtr);
  cr_assert(infoPtr->allocatedBlocks == 1, "allocatedBlocks is not 1");
  cr_assert(infoPtr->splinterBlocks == 0, "splinterBlocks is not 0");
  cr_assert(infoPtr->padding == 1, "padding is not 1");
  cr_assert(infoPtr->splintering == 0, "splintering is not 0");
  cr_assert(infoPtr->coalesces == 0, "coalesces is not 0");
  cr_assert(infoPtr->peakMemoryUtilization == 15.0/4096.0, "peakMemoryUtilization is not 15/4096");
}

Test(sf_memsuite, Test53_realloc_same_block_4, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(14);
  *a = 35;
  char *b = sf_realloc(a, 15);
  cr_assert(b == a, "b != a");
  cr_assert(*b == 35, "did not sign value");
  cr_assert(freelist_head != NULL, "freelist_head is NULL");
  cr_assert(freelist_head->next == NULL, "freelist_head->next != NULL");
  cr_assert(freelist_head->prev == NULL, "freelist_head->prev != NULL");
  cr_assert(freelist_head == (void*)(a + 24), "Address is not correct.");
  cr_assert(freelist_head->header.alloc == 0, "freelist_head alloc is not 0");
  cr_assert(freelist_head->header.splinter == 0, "freelist_head splinter is not 0");
  cr_assert(freelist_head->header.block_size == 4064 >> 4, "freelist_head block_size is not 4064 >> 4");
  sf_footer* freelist_head_footer = (sf_footer*)((char*) freelist_head + ((freelist_head->header.block_size) << 4) - 8);
  cr_assert(freelist_head_footer->alloc == 0, "freelist_head footer alloc is not 0");
  cr_assert(freelist_head_footer->splinter == 0, "freelist_head footer splinter is not 0");
  cr_assert(freelist_head_footer->block_size == 4064 >> 4, "freelist_head footer block_size is not 4064 >> 4");
  sf_header* bHeader = (sf_header*)(b - 8);
  cr_assert(bHeader->alloc == 1, "bHeader alloc is not 1");
  cr_assert(bHeader->splinter == 0, "bHeader splinter is not 0");
  cr_assert(bHeader->block_size == 32 >> 4, "bHeader block_size is not 32 >> 4");
  cr_assert(bHeader->requested_size == 15, "bHeader requested_size is not 15");
  cr_assert(bHeader->splinter_size == 0, "bHeader splinter_size is not 0");
  cr_assert(bHeader->padding_size == 1, "bHeader padding_size is not 1");
  sf_footer* bFooter = (sf_footer*)(b + (bHeader->block_size << 4) - 16);
  cr_assert(bFooter->alloc == 1, "bFooter alloc is not 1");
  cr_assert(bFooter->splinter == 0, "bFooter splinter is not 0");
  cr_assert(bFooter->block_size == 32 >> 4, "bFooter block_size is not 32 >> 4");
  info INFORM;
  info* infoPtr = &INFORM;
  sf_info(infoPtr);
  cr_assert(infoPtr->allocatedBlocks == 1, "allocatedBlocks is not 1");
  cr_assert(infoPtr->splinterBlocks == 0, "splinterBlocks is not 0");
  cr_assert(infoPtr->padding == 1, "padding is not 1");
  cr_assert(infoPtr->splintering == 0, "splintering is not 0");
  cr_assert(infoPtr->coalesces == 0, "coalesces is not 0");
  cr_assert(infoPtr->peakMemoryUtilization == 15.0/4096.0, "peakMemoryUtilization is not 15/4096");
}

Test(sf_memsuite, Test54_realloc_same_block_4, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(32);
  char *c = sf_malloc(16);
  sf_free(a);
  a = sf_malloc(14);
  *a = 35;
  char *b = sf_realloc(a, 18);
  cr_assert(b == a, "b != a");
  cr_assert(*b == 35, "did not sign value");
  cr_assert(freelist_head != NULL, "freelist_head is NULL");
  cr_assert(freelist_head->next == NULL, "freelist_head->next != NULL");
  cr_assert(freelist_head->prev == NULL, "freelist_head->prev != NULL");
  cr_assert(freelist_head == (void*)(a + 32 + 16 + 16 + 8), "Address is not correct.");
  cr_assert(freelist_head->header.alloc == 0, "freelist_head alloc is not 0");
  cr_assert(freelist_head->header.splinter == 0, "freelist_head splinter is not 0");
  cr_assert(freelist_head->header.block_size == (4096 - 32 - 16 - 16 - 16) >> 4, "freelist_head block_size is not (4096 - 32 - 16 - 16 - 16) >> 4");
  sf_footer* freelist_head_footer = (sf_footer*)((char*) freelist_head + ((freelist_head->header.block_size) << 4) - 8);
  cr_assert(freelist_head_footer->alloc == 0, "freelist_head footer alloc is not 0");
  cr_assert(freelist_head_footer->splinter == 0, "freelist_head footer splinter is not 0");
  cr_assert(freelist_head_footer->block_size == (4096 - 32 - 16 - 16 - 16) >> 4, "freelist_head footer block_size is not (4096 - 32 - 16 - 16 - 16) >> 4");
  sf_header* bHeader = (sf_header*)(b - 8);
  cr_assert(bHeader->alloc == 1, "bHeader alloc is not 1");
  cr_assert(bHeader->splinter == 0, "bHeader splinter is not 0");
  cr_assert(bHeader->block_size == 48 >> 4, "bHeader block_size is not 48 >> 4");
  cr_assert(bHeader->requested_size == 18, "bHeader requested_size is not 18");
  cr_assert(bHeader->splinter_size == 0, "bHeader splinter_size is not 0");
  cr_assert(bHeader->padding_size == 14, "bHeader padding_size is not 14");
  sf_footer* bFooter = (sf_footer*)(b + (bHeader->block_size << 4) - 16);
  cr_assert(bFooter->alloc == 1, "bFooter alloc is not 1");
  cr_assert(bFooter->splinter == 0, "bFooter splinter is not 0");
  cr_assert(bFooter->block_size == 48 >> 4, "bFooter block_size is not 48 >> 4");
  info INFORM;
  info* infoPtr = &INFORM;
  sf_info(infoPtr);
  cr_assert(infoPtr->allocatedBlocks == 2, "allocatedBlocks is not 2");
  cr_assert(infoPtr->splinterBlocks == 0, "splinterBlocks is not 0");
  cr_assert(infoPtr->padding == 14, "padding is not 14");
  cr_assert(infoPtr->splintering == 0, "splintering is not 0");
  cr_assert(infoPtr->coalesces == 0, "coalesces is not 0");
  cr_assert(infoPtr->peakMemoryUtilization == 48.0/4096.0, "peakMemoryUtilization is not 48/4096");
  sf_free(c);
}

Test(sf_memsuite, Test55_realloc_smaller_block_1, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(128);
  char *c = sf_malloc(16);
  *a = 37;
  char *b = sf_realloc(a, 64);
  cr_assert(b == a, "b != a");
  cr_assert(*b == 37, "did not sign value");
  cr_assert(freelist_head != NULL, "freelist_head is NULL");
  cr_assert(freelist_head->next != NULL, "freelist_head->next == NULL");
  cr_assert(freelist_head->prev == NULL, "freelist_head->prev != NULL");
  cr_assert(freelist_head == (void*)(a + 64 + 8), "Address is not correct.");
  cr_assert(freelist_head->header.alloc == 0, "freelist_head alloc is not 0");
  cr_assert(freelist_head->header.splinter == 0, "freelist_head splinter is not 0");
  cr_assert(freelist_head->header.block_size == 64 >> 4, "freelist_head block_size is not 64 >> 4");
  sf_footer* freelist_head_footer = (sf_footer*)((char*) freelist_head + ((freelist_head->header.block_size) << 4) - 8);
  cr_assert(freelist_head_footer->alloc == 0, "freelist_head footer alloc is not 0");
  cr_assert(freelist_head_footer->splinter == 0, "freelist_head footer splinter is not 0");
  cr_assert(freelist_head_footer->block_size == 64 >> 4, "freelist_head footer block_size is not 64 >> 4");

  sf_free_header* hd2 = freelist_head->next;
  cr_assert(hd2 != NULL, "freelist_head is NULL");
  cr_assert(hd2->next == NULL, "hd2->next != NULL");
  cr_assert(hd2->prev == freelist_head, "hd2->prev != freelist_head");
  cr_assert(hd2 == (void*)(a + 128 + 16 + 16 + 8), "hd2 Address is not correct.");
  cr_assert(hd2->header.alloc == 0, "hd2 alloc is not 0");
  cr_assert(hd2->header.splinter == 0, "hd2 splinter is not 0");
  cr_assert(hd2->header.block_size == (4096 - 128 - 16 - 32) >> 4, "hd2 block_size is not (4096 - 128 - 16 - 32) >> 4");
  sf_footer* ft2 = (sf_footer*)((char*) hd2 + ((hd2->header.block_size) << 4) - 8);
  cr_assert(ft2->alloc == 0, "ft2 alloc is not 0");
  cr_assert(ft2->splinter == 0, "ft2 splinter is not 0");
  cr_assert(ft2->block_size == (4096 - 128 - 16 - 32) >> 4, "ft2 block_size is not (4096 - 128 - 16 - 32) >> 4");

  sf_header* bHeader = (sf_header*)(b - 8);
  cr_assert(bHeader->alloc == 1, "bHeader alloc is not 1");
  cr_assert(bHeader->splinter == 0, "bHeader splinter is not 0");
  cr_assert(bHeader->block_size == 80 >> 4, "bHeader block_size is not 80 >> 4");
  cr_assert(bHeader->requested_size == 64, "bHeader requested_size is not 64");
  cr_assert(bHeader->splinter_size == 0, "bHeader splinter_size is not 0");
  cr_assert(bHeader->padding_size == 0, "bHeader padding_size is not 0");
  sf_footer* bFooter = (sf_footer*)(b + (bHeader->block_size << 4) - 16);
  cr_assert(bFooter->alloc == 1, "bFooter alloc is not 1");
  cr_assert(bFooter->splinter == 0, "bFooter splinter is not 0");
  cr_assert(bFooter->block_size == 80 >> 4, "bFooter block_size is not 80 >> 4");
  info INFORM;
  info* infoPtr = &INFORM;
  sf_info(infoPtr);
  cr_assert(infoPtr->allocatedBlocks == 2, "allocatedBlocks is not 2");
  cr_assert(infoPtr->splinterBlocks == 0, "splinterBlocks is not 0");
  cr_assert(infoPtr->padding == 0, "padding is not 0");
  cr_assert(infoPtr->splintering == 0, "splintering is not 0");
  cr_assert(infoPtr->coalesces == 0, "coalesces is not 0");
  cr_assert(infoPtr->peakMemoryUtilization == 144.0/4096.0, "peakMemoryUtilization is not 144/4096");
  sf_free(c);
}

Test(sf_memsuite, Test56_realloc_smaller_block_2, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(128);
  *a = 37;
  char *b = sf_realloc(a, 64);
  cr_assert(b == a, "b != a");
  cr_assert(*b == 37, "did not sign value");
  cr_assert(freelist_head != NULL, "freelist_head is NULL");
  cr_assert(freelist_head->next == NULL, "freelist_head->next != NULL");
  cr_assert(freelist_head->prev == NULL, "freelist_head->prev != NULL");
  cr_assert(freelist_head == (void*)(a + 64 + 8), "Address is not correct.");
  cr_assert(freelist_head->header.alloc == 0, "freelist_head alloc is not 0");
  cr_assert(freelist_head->header.splinter == 0, "freelist_head splinter is not 0");
  cr_assert(freelist_head->header.block_size == (4096 - 64 - 16) >> 4, "freelist_head block_size is not (4096 - 64 - 16) >> 4");
  sf_footer* freelist_head_footer = (sf_footer*)((char*) freelist_head + ((freelist_head->header.block_size) << 4) - 8);
  cr_assert(freelist_head_footer->alloc == 0, "freelist_head footer alloc is not 0");
  cr_assert(freelist_head_footer->splinter == 0, "freelist_head footer splinter is not 0");
  cr_assert(freelist_head_footer->block_size == (4096 - 64 - 16) >> 4, "freelist_head footer block_size is not (4096 - 64 - 16) >> 4");

  sf_header* bHeader = (sf_header*)(b - 8);
  cr_assert(bHeader->alloc == 1, "bHeader alloc is not 1");
  cr_assert(bHeader->splinter == 0, "bHeader splinter is not 0");
  cr_assert(bHeader->block_size == 80 >> 4, "bHeader block_size is not 80 >> 4");
  cr_assert(bHeader->requested_size == 64, "bHeader requested_size is not 64");
  cr_assert(bHeader->splinter_size == 0, "bHeader splinter_size is not 0");
  cr_assert(bHeader->padding_size == 0, "bHeader padding_size is not 0");
  sf_footer* bFooter = (sf_footer*)(b + (bHeader->block_size << 4) - 16);
  cr_assert(bFooter->alloc == 1, "bFooter alloc is not 1");
  cr_assert(bFooter->splinter == 0, "bFooter splinter is not 0");
  cr_assert(bFooter->block_size == 80 >> 4, "bFooter block_size is not 80 >> 4");
  info INFORM;
  info* infoPtr = &INFORM;
  sf_info(infoPtr);
  cr_assert(infoPtr->allocatedBlocks == 1, "allocatedBlocks is not 1");
  cr_assert(infoPtr->splinterBlocks == 0, "splinterBlocks is not 0");
  cr_assert(infoPtr->padding == 0, "padding is not 0");
  cr_assert(infoPtr->splintering == 0, "splintering is not 0");
  cr_assert(infoPtr->coalesces == 1, "coalesces is not 1");
  cr_assert(infoPtr->peakMemoryUtilization == 128.0/4096.0, "peakMemoryUtilization is not 128/4096");
}

Test(sf_memsuite, Test57_realloc_smaller_block_3, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(4080);
  *a = 37;
  char *b = sf_realloc(a, 64);
  cr_assert(b == a, "b != a");
  cr_assert(*b == 37, "did not sign value");
  cr_assert(freelist_head != NULL, "freelist_head is NULL");
  cr_assert(freelist_head->next == NULL, "freelist_head->next != NULL");
  cr_assert(freelist_head->prev == NULL, "freelist_head->prev != NULL");
  cr_assert(freelist_head == (void*)(a + 64 + 8), "Address is not correct.");
  cr_assert(freelist_head->header.alloc == 0, "freelist_head alloc is not 0");
  cr_assert(freelist_head->header.splinter == 0, "freelist_head splinter is not 0");
  cr_assert(freelist_head->header.block_size == (4096 - 64 - 16) >> 4, "freelist_head block_size is not (4096 - 64 - 16) >> 4");
  sf_footer* freelist_head_footer = (sf_footer*)((char*) freelist_head + ((freelist_head->header.block_size) << 4) - 8);
  cr_assert(freelist_head_footer->alloc == 0, "freelist_head footer alloc is not 0");
  cr_assert(freelist_head_footer->splinter == 0, "freelist_head footer splinter is not 0");
  cr_assert(freelist_head_footer->block_size == (4096 - 64 - 16) >> 4, "freelist_head footer block_size is not (4096 - 64 - 16) >> 4");

  sf_header* bHeader = (sf_header*)(b - 8);
  cr_assert(bHeader->alloc == 1, "bHeader alloc is not 1");
  cr_assert(bHeader->splinter == 0, "bHeader splinter is not 0");
  cr_assert(bHeader->block_size == 80 >> 4, "bHeader block_size is not 80 >> 4");
  cr_assert(bHeader->requested_size == 64, "bHeader requested_size is not 64");
  cr_assert(bHeader->splinter_size == 0, "bHeader splinter_size is not 0");
  cr_assert(bHeader->padding_size == 0, "bHeader padding_size is not 0");
  sf_footer* bFooter = (sf_footer*)(b + (bHeader->block_size << 4) - 16);
  cr_assert(bFooter->alloc == 1, "bFooter alloc is not 1");
  cr_assert(bFooter->splinter == 0, "bFooter splinter is not 0");
  cr_assert(bFooter->block_size == 80 >> 4, "bFooter block_size is not 80 >> 4");
  info INFORM;
  info* infoPtr = &INFORM;
  sf_info(infoPtr);
  cr_assert(infoPtr->allocatedBlocks == 1, "allocatedBlocks is not 1");
  cr_assert(infoPtr->splinterBlocks == 0, "splinterBlocks is not 0");
  cr_assert(infoPtr->padding == 0, "padding is not 0");
  cr_assert(infoPtr->splintering == 0, "splintering is not 0");
  cr_assert(infoPtr->coalesces == 0, "coalesces is not 0");
  cr_assert(infoPtr->peakMemoryUtilization == 4080.0/4096.0, "peakMemoryUtilization is not 4080/4096");
}

Test(sf_memsuite, Test58_realloc_larger_block_1, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(16);
  *a = 38;
  char *b = sf_realloc(a, 32);
  cr_assert(b == a, "b != a");
  cr_assert(*b == 38, "did not sign value");
  cr_assert(freelist_head != NULL, "freelist_head is NULL");
  cr_assert(freelist_head->next == NULL, "freelist_head->next != NULL");
  cr_assert(freelist_head->prev == NULL, "freelist_head->prev != NULL");
  cr_assert(freelist_head == (void*)(a + 32 + 8), "Address is not correct.");
  cr_assert(freelist_head->header.alloc == 0, "freelist_head alloc is not 0");
  cr_assert(freelist_head->header.splinter == 0, "freelist_head splinter is not 0");
  cr_assert(freelist_head->header.block_size == (4096 - 32 - 16) >> 4, "freelist_head block_size is not (4096 - 32 - 16) >> 4");
  sf_footer* freelist_head_footer = (sf_footer*)((char*) freelist_head + ((freelist_head->header.block_size) << 4) - 8);
  cr_assert(freelist_head_footer->alloc == 0, "freelist_head footer alloc is not 0");
  cr_assert(freelist_head_footer->splinter == 0, "freelist_head footer splinter is not 0");
  cr_assert(freelist_head_footer->block_size == (4096 - 32 - 16) >> 4, "freelist_head footer block_size is not (4096 - 32 - 16) >> 4");

  sf_header* bHeader = (sf_header*)(b - 8);
  cr_assert(bHeader->alloc == 1, "bHeader alloc is not 1");
  cr_assert(bHeader->splinter == 0, "bHeader splinter is not 0");
  cr_assert(bHeader->block_size == 48 >> 4, "bHeader block_size is not 48 >> 4");
  cr_assert(bHeader->requested_size == 32, "bHeader requested_size is not 32");
  cr_assert(bHeader->splinter_size == 0, "bHeader splinter_size is not 0");
  cr_assert(bHeader->padding_size == 0, "bHeader padding_size is not 0");
  sf_footer* bFooter = (sf_footer*)(b + (bHeader->block_size << 4) - 16);
  cr_assert(bFooter->alloc == 1, "bFooter alloc is not 1");
  cr_assert(bFooter->splinter == 0, "bFooter splinter is not 0");
  cr_assert(bFooter->block_size == 48 >> 4, "bFooter block_size is not 48 >> 4");
  info INFORM;
  info* infoPtr = &INFORM;
  sf_info(infoPtr);
  cr_assert(infoPtr->allocatedBlocks == 1, "allocatedBlocks is not 1");
  cr_assert(infoPtr->splinterBlocks == 0, "splinterBlocks is not 0");
  cr_assert(infoPtr->padding == 0, "padding is not 0");
  cr_assert(infoPtr->splintering == 0, "splintering is not 0");
  cr_assert(infoPtr->coalesces == 0, "coalesces is not 0");
  cr_assert(infoPtr->peakMemoryUtilization == 32.0/4096.0, "peakMemoryUtilization is not 32/4096");
}

Test(sf_memsuite, Test59_realloc_larger_block_2, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(4080);
  *a = 39;
  char *b = sf_realloc(a, 4096);
  cr_assert(b == a, "b != a");
  cr_assert(*b == 39, "did not sign value");
  cr_assert(freelist_head != NULL, "freelist_head is NULL");
  cr_assert(freelist_head->next == NULL, "freelist_head->next != NULL");
  cr_assert(freelist_head->prev == NULL, "freelist_head->prev != NULL");
  cr_assert(freelist_head == (void*)(a + 4096 + 8), "Address is not correct.");
  cr_assert(freelist_head->header.alloc == 0, "freelist_head alloc is not 0");
  cr_assert(freelist_head->header.splinter == 0, "freelist_head splinter is not 0");
  cr_assert(freelist_head->header.block_size == (4096 - 16) >> 4, "freelist_head block_size is not (4096 - 16) >> 4");
  sf_footer* freelist_head_footer = (sf_footer*)((char*) freelist_head + ((freelist_head->header.block_size) << 4) - 8);
  cr_assert(freelist_head_footer->alloc == 0, "freelist_head footer alloc is not 0");
  cr_assert(freelist_head_footer->splinter == 0, "freelist_head footer splinter is not 0");
  cr_assert(freelist_head_footer->block_size == (4096 - 16) >> 4, "freelist_head footer block_size is not (4096 - 16) >> 4");

  sf_header* bHeader = (sf_header*)(b - 8);
  cr_assert(bHeader->alloc == 1, "bHeader alloc is not 1");
  cr_assert(bHeader->splinter == 0, "bHeader splinter is not 0");
  cr_assert(bHeader->block_size == 4112 >> 4, "bHeader block_size is not 4112 >> 4");
  cr_assert(bHeader->requested_size == 4096, "bHeader requested_size is not 4096");
  cr_assert(bHeader->splinter_size == 0, "bHeader splinter_size is not 0");
  cr_assert(bHeader->padding_size == 0, "bHeader padding_size is not 0");
  sf_footer* bFooter = (sf_footer*)(b + (bHeader->block_size << 4) - 16);
  cr_assert(bFooter->alloc == 1, "bFooter alloc is not 1");
  cr_assert(bFooter->splinter == 0, "bFooter splinter is not 0");
  cr_assert(bFooter->block_size == 4112 >> 4, "bFooter block_size is not 4112 >> 4");
  info INFORM;
  info* infoPtr = &INFORM;
  sf_info(infoPtr);
  cr_assert(infoPtr->allocatedBlocks == 1, "allocatedBlocks is not 1");
  cr_assert(infoPtr->splinterBlocks == 0, "splinterBlocks is not 0");
  cr_assert(infoPtr->padding == 0, "padding is not 0");
  cr_assert(infoPtr->splintering == 0, "splintering is not 0");
  cr_assert(infoPtr->coalesces == 0, "coalesces is not 0");
  cr_assert(infoPtr->peakMemoryUtilization == 4096.0/8192.0, "peakMemoryUtilization is not 4096/8192");
}

Test(sf_memsuite, Test60_realloc_larger_block_3, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(40);
  *a = 40;
  char *b = sf_realloc(a, 4096);
  cr_assert(b == a, "b != a");
  cr_assert(*b == 40, "did not sign value");
  cr_assert(freelist_head != NULL, "freelist_head is NULL");
  cr_assert(freelist_head->next == NULL, "freelist_head->next != NULL");
  cr_assert(freelist_head->prev == NULL, "freelist_head->prev != NULL");
  cr_assert(freelist_head == (void*)(a + 4096 + 8), "Address is not correct.");
  cr_assert(freelist_head->header.alloc == 0, "freelist_head alloc is not 0");
  cr_assert(freelist_head->header.splinter == 0, "freelist_head splinter is not 0");
  cr_assert(freelist_head->header.block_size == (4096 - 16) >> 4, "freelist_head block_size is not (4096 - 16) >> 4");
  sf_footer* freelist_head_footer = (sf_footer*)((char*) freelist_head + ((freelist_head->header.block_size) << 4) - 8);
  cr_assert(freelist_head_footer->alloc == 0, "freelist_head footer alloc is not 0");
  cr_assert(freelist_head_footer->splinter == 0, "freelist_head footer splinter is not 0");
  cr_assert(freelist_head_footer->block_size == (4096 - 16) >> 4, "freelist_head footer block_size is not (4096 - 16) >> 4");

  sf_header* bHeader = (sf_header*)(b - 8);
  cr_assert(bHeader->alloc == 1, "bHeader alloc is not 1");
  cr_assert(bHeader->splinter == 0, "bHeader splinter is not 0");
  cr_assert(bHeader->block_size == 4112 >> 4, "bHeader block_size is not 4112 >> 4");
  cr_assert(bHeader->requested_size == 4096, "bHeader requested_size is not 4096");
  cr_assert(bHeader->splinter_size == 0, "bHeader splinter_size is not 0");
  cr_assert(bHeader->padding_size == 0, "bHeader padding_size is not 0");
  sf_footer* bFooter = (sf_footer*)(b + (bHeader->block_size << 4) - 16);
  cr_assert(bFooter->alloc == 1, "bFooter alloc is not 1");
  cr_assert(bFooter->splinter == 0, "bFooter splinter is not 0");
  cr_assert(bFooter->block_size == 4112 >> 4, "bFooter block_size is not 4112 >> 4");
  info INFORM;
  info* infoPtr = &INFORM;
  sf_info(infoPtr);
  cr_assert(infoPtr->allocatedBlocks == 1, "allocatedBlocks is not 1");
  cr_assert(infoPtr->splinterBlocks == 0, "splinterBlocks is not 0");
  cr_assert(infoPtr->padding == 0, "padding is not 0");
  cr_assert(infoPtr->splintering == 0, "splintering is not 0");
  cr_assert(infoPtr->coalesces == 1, "coalesces is not 1");
  cr_assert(infoPtr->peakMemoryUtilization == 4096.0/8192.0, "peakMemoryUtilization is not 4096/8192");
}

Test(sf_memsuite, Test61_realloc_larger_block_4, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(40);
  *a = 41;
  char *b = sf_realloc(a, 4080);
  cr_assert(b == a, "b != a");
  cr_assert(*b == 41, "did not sign value");
  cr_assert(freelist_head == NULL, "freelist_head is not NULL");

  sf_header* bHeader = (sf_header*)(b - 8);
  cr_assert(bHeader->alloc == 1, "bHeader alloc is not 1");
  cr_assert(bHeader->splinter == 0, "bHeader splinter is not 0");
  cr_assert(bHeader->block_size == 4096 >> 4, "bHeader block_size is not 4096 >> 4");
  cr_assert(bHeader->requested_size == 4080, "bHeader requested_size is not 4080");
  cr_assert(bHeader->splinter_size == 0, "bHeader splinter_size is not 0");
  cr_assert(bHeader->padding_size == 0, "bHeader padding_size is not 0");
  sf_footer* bFooter = (sf_footer*)(b + (bHeader->block_size << 4) - 16);
  cr_assert(bFooter->alloc == 1, "bFooter alloc is not 1");
  cr_assert(bFooter->splinter == 0, "bFooter splinter is not 0");
  cr_assert(bFooter->block_size == 4096 >> 4, "bFooter block_size is not 4096 >> 4");
  info INFORM;
  info* infoPtr = &INFORM;
  sf_info(infoPtr);
  cr_assert(infoPtr->allocatedBlocks == 1, "allocatedBlocks is not 1");
  cr_assert(infoPtr->splinterBlocks == 0, "splinterBlocks is not 0");
  cr_assert(infoPtr->padding == 0, "padding is not 0");
  cr_assert(infoPtr->splintering == 0, "splintering is not 0");
  cr_assert(infoPtr->coalesces == 0, "coalesces is not 0");
  cr_assert(infoPtr->peakMemoryUtilization == 4080.0/4096.0, "peakMemoryUtilization is not 4080/4096");
}

Test(sf_memsuite, Test62_realloc_larger_block_5, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(40);
  *a = 42;
  char *b = sf_realloc(a, 4064);
  cr_assert(b == a, "b != a");
  cr_assert(*b == 42, "did not sign value");
  cr_assert(freelist_head == NULL, "freelist_head is not NULL");

  sf_header* bHeader = (sf_header*)(b - 8);
  cr_assert(bHeader->alloc == 1, "bHeader alloc is not 1");
  cr_assert(bHeader->splinter == 1, "bHeader splinter is not 1");
  cr_assert(bHeader->block_size == 4096 >> 4, "bHeader block_size is not 4096 >> 4");
  cr_assert(bHeader->requested_size == 4064, "bHeader requested_size is not 4064");
  cr_assert(bHeader->splinter_size == 16, "bHeader splinter_size is not 16");
  cr_assert(bHeader->padding_size == 0, "bHeader padding_size is not 0");
  sf_footer* bFooter = (sf_footer*)(b + (bHeader->block_size << 4) - 16);
  cr_assert(bFooter->alloc == 1, "bFooter alloc is not 1");
  cr_assert(bFooter->splinter == 1, "bFooter splinter is not 1");
  cr_assert(bFooter->block_size == 4096 >> 4, "bFooter block_size is not 4096 >> 4");
  info INFORM;
  info* infoPtr = &INFORM;
  sf_info(infoPtr);
  cr_assert(infoPtr->allocatedBlocks == 1, "allocatedBlocks is not 1");
  cr_assert(infoPtr->splinterBlocks == 1, "splinterBlocks is not 1");
  cr_assert(infoPtr->padding == 0, "padding is not 0");
  cr_assert(infoPtr->splintering == 16, "splintering is not 16");
  cr_assert(infoPtr->coalesces == 0, "coalesces is not 0");
  cr_assert(infoPtr->peakMemoryUtilization == 4064.0/4096.0, "peakMemoryUtilization is not 4064/4096");
}

Test(sf_memsuite, Test63_realloc_larger_block_6, .init = sf_mem_init, .fini = sf_mem_fini) {
  char *a = sf_malloc(16);
  char *c = sf_malloc(16);
  *a = 37;
  char *b = sf_realloc(a, 64);
  cr_assert(b != a, "b == a");
  cr_assert(*b == 37, "did not sign value");
  cr_assert(freelist_head != NULL, "freelist_head is NULL");
  cr_assert(freelist_head->next != NULL, "freelist_head->next == NULL");
  cr_assert(freelist_head->prev == NULL, "freelist_head->prev != NULL");
  cr_assert(freelist_head == (void*)(a - 8), "Address is not correct.");
  cr_assert(freelist_head->header.alloc == 0, "freelist_head alloc is not 0");
  cr_assert(freelist_head->header.splinter == 0, "freelist_head splinter is not 0");
  cr_assert(freelist_head->header.block_size == 32 >> 4, "freelist_head block_size is not 32 >> 4");
  sf_footer* freelist_head_footer = (sf_footer*)((char*) freelist_head + ((freelist_head->header.block_size) << 4) - 8);
  cr_assert(freelist_head_footer->alloc == 0, "freelist_head footer alloc is not 0");
  cr_assert(freelist_head_footer->splinter == 0, "freelist_head footer splinter is not 0");
  cr_assert(freelist_head_footer->block_size == 32 >> 4, "freelist_head footer block_size is not 32 >> 4");

  sf_free_header* hd2 = freelist_head->next;
  cr_assert(hd2 != NULL, "freelist_head is NULL");
  cr_assert(hd2->next == NULL, "hd2->next != NULL");
  cr_assert(hd2->prev == freelist_head, "hd2->prev != freelist_head");
  cr_assert(hd2 == (void*)(a + 128 + 8), "hd2 Address is not correct.");
  cr_assert(hd2->header.alloc == 0, "hd2 alloc is not 0");
  cr_assert(hd2->header.splinter == 0, "hd2 splinter is not 0");
  cr_assert(hd2->header.block_size == (4096 - 128 - 16) >> 4, "hd2 block_size is not (4096 - 128 - 16) >> 4");
  sf_footer* ft2 = (sf_footer*)((char*) hd2 + ((hd2->header.block_size) << 4) - 8);
  cr_assert(ft2->alloc == 0, "ft2 alloc is not 0");
  cr_assert(ft2->splinter == 0, "ft2 splinter is not 0");
  cr_assert(ft2->block_size == (4096 - 128 - 16) >> 4, "ft2 block_size is not (4096 - 128 - 16) >> 4");

  sf_header* bHeader = (sf_header*)(b - 8);
  cr_assert(bHeader->alloc == 1, "bHeader alloc is not 1");
  cr_assert(bHeader->splinter == 0, "bHeader splinter is not 0");
  cr_assert(bHeader->block_size == 80 >> 4, "bHeader block_size is not 80 >> 4");
  cr_assert(bHeader->requested_size == 64, "bHeader requested_size is not 64");
  cr_assert(bHeader->splinter_size == 0, "bHeader splinter_size is not 0");
  cr_assert(bHeader->padding_size == 0, "bHeader padding_size is not 0");
  sf_footer* bFooter = (sf_footer*)(b + (bHeader->block_size << 4) - 16);
  cr_assert(bFooter->alloc == 1, "bFooter alloc is not 1");
  cr_assert(bFooter->splinter == 0, "bFooter splinter is not 0");
  cr_assert(bFooter->block_size == 80 >> 4, "bFooter block_size is not 80 >> 4");
  info INFORM;
  info* infoPtr = &INFORM;
  sf_info(infoPtr);
  cr_assert(infoPtr->allocatedBlocks == 2, "allocatedBlocks is not 2");
  cr_assert(infoPtr->splinterBlocks == 0, "splinterBlocks is not 0");
  cr_assert(infoPtr->padding == 0, "padding is not 0");
  cr_assert(infoPtr->splintering == 0, "splintering is not 0");
  cr_assert(infoPtr->coalesces == 0, "coalesces is not 0");
  cr_assert(infoPtr->peakMemoryUtilization == 80.0/4096.0, "peakMemoryUtilization is not 80/4096, it is %lf", infoPtr->peakMemoryUtilization * 4096.0);
  sf_free(c);
}

Test(sf_memsuite, Test64_invalid_realloc, .init = sf_mem_init, .fini = sf_mem_fini) {
  char * ptr = sf_realloc(NULL, 24);
  cr_assert(ptr == NULL, "ptr is not NULL");
  cr_assert(errno == EINVAL, "errno is not EINVAL");
}


Test(sf_memsuite, Test65_invalid_free, .init = sf_mem_init, .fini = sf_mem_fini) {
  sf_free(NULL);
  cr_assert(errno == EINVAL, "errno is not EINVAL");
}




