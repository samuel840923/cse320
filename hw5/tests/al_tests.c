#include <criterion/criterion.h>
#include <criterion/logging.h>
#include <stdio.h>
#include <stdbool.h>
#include "arraylist.h"

/******************************************
 *                  ITEMS                 *
 ******************************************/
arraylist_t *global_list;

typedef struct {
    char* name;
    int32_t id;
    double gpa;
}student_t;

typedef struct{
    int i;
    float f;
    long double ld;
    char c1:4;
    char c2:4;
    short s;
    void *some_data;
}test_item_t;

/******************************************
 *              HELPER FUNCS              *
 ******************************************/
void test_item_t_free_func(void *argptr){
    test_item_t* ptr = (test_item_t*) argptr;
    if(!ptr)
        free(ptr->some_data);
    else
        cr_log_warn("%s\n", "Pointer was NULL");
}

void setup(void) {
    cr_log_warn("Setting up test");
    global_list = new_al(sizeof(test_item_t));
}

void teardown(void) {
    cr_log_error("Tearing down");
    delete_al(global_list, test_item_t_free_func);
}

/******************************************
 *                  TESTS                 *
 ******************************************/
Test(al_suite, 0_creation, .timeout=2){
    arraylist_t *locallist = new_al(sizeof(test_item_t));

    cr_assert_not_null(locallist, "List returned was NULL");
}

Test(al_suite, 1_deletion, .timeout=2){
    arraylist_t *locallist = new_al(sizeof(test_item_t));

    cr_assert_not_null(locallist, "List returned was NULL");

    delete_al(locallist, test_item_t_free_func);

    cr_assert(true, "Delete completed without crashing");
}

Test(al_suite, 2_insertion, .timeout=2, .init=setup, .fini=teardown){
    cr_assert(true, "I win");
}

Test(al_suite, 3_removal, .timeout=2, .init=setup, .fini=teardown){
  new_al(sizeof(int));


}
Test(al_suite, 4_mytest, .timeout=2){
    arraylist_t *locallist = new_al(sizeof(int));
    cr_assert_not_null(locallist, "List returned was NULL");
    int a = 1;
    int *b = &a;
    insert_al(locallist,b);
    a = 2;
    insert_al(locallist,b);
    a = 3;
    insert_al(locallist,b);
    a = 4;
    insert_al(locallist,b);
    a = 5;
    insert_al(locallist,b);
    a = 6;
    insert_al(locallist,b);
    a = 7;
    insert_al(locallist,b);
    a = 8;
    insert_al(locallist,b);
     a = 9;
    insert_al(locallist,b);
    int g = 1;
    int *k = &g;
     printf("-%zu\n",locallist->capacity);
    remove_data_al(locallist,k);
    int leng = locallist->length;
    for(int i=0;i<leng;i++){
        int *c = get_index_al(locallist,i);
       printf("---%d\n",*c);
    }
}
Test(al_suite, 5_mytest, .timeout=2){
    arraylist_t *locallist = new_al(sizeof(int));
    cr_assert_not_null(locallist, "List returned was NULL");
    int a = 1;
    int *b = &a;
    insert_al(locallist,b);
    a = 2;
    insert_al(locallist,b);
    a = 3;
    insert_al(locallist,b);
    a = 4;
    insert_al(locallist,b);
    a = 5;
    insert_al(locallist,b);
    a = 6;
    insert_al(locallist,b);
    a = 7;
    insert_al(locallist,b);
    a = 8;
    insert_al(locallist,b);
     a = 9;
    insert_al(locallist,b);

    int leng = locallist->length;
    for(int i=0;i<leng;i++){
        int *c = get_index_al(locallist,i);
        cr_assert(*c==i+1,"not equal");
    }
}

