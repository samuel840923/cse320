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
  cr_assert(ptr->coalesces==1, "1 coal");
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
  cr_assert(ptr->coalesces==1, "1 coal");
  cr_assert((*((sf_header*)test)).block_size<<4==96, "new realloc should be 96");
  cr_assert((*((sf_header*)test)).padding_size==0, "0 padding needed");
   cr_assert(ptr->padding==5,"total padding is 5");
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
sf_free(a);
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



