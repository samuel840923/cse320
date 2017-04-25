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
void student_t_free_func(void *argptr){
    student_t* ptr = (student_t*) argptr;
    if(ptr != NULL)
        free(ptr->name);
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
    arraylist_t *self = new_al(sizeof(int));
     int a =0;
     int*b =&a;
     insert_al(self,b);
     a = 8;
     insert_al(self,b);
     a = 9;
     insert_al(self,b);
     a = 10;
     insert_al(self,b);
     a = 11;
     insert_al(self,b);
     a = 12;
     insert_al(self,b);
     int *test = get_index_al(self,4);
     cr_assert(*test==11,"no good");
   test = get_index_al(self,5);
     cr_assert(*test==12,"no good");
     test = get_index_al(self,3);
     cr_assert(*test==10,"no good");
     test = get_index_al(self,2);
     cr_assert(*test==9,"no good");
      test = get_index_al(self,1);
     cr_assert(*test==8,"no good");
     test = get_index_al(self,0);
     cr_assert(*test==0,"no good");
     cr_assert(self->capacity==8,"no good");
}

Test(al_suite, 1_deletion, .timeout=2){
   arraylist_t *self = new_al(sizeof(int));
     int a =0;
     int*b =&a;
     insert_al(self,b);
     a = 8;
     insert_al(self,b);
     a = 9;
     insert_al(self,b);
     a = 10;
     insert_al(self,b);
     a = 11;
     insert_al(self,b);
     a = 12;
     insert_al(self,b);
     int no = 1;
     int* te = &no;
     bool check = remove_data_al(self,te);
     cr_assert(check==false, "cannot find");
     cr_assert(self->length==6, "cannot find");
     no = 8;
      check = remove_data_al(self,te);
      cr_assert(check==true, "cannot find");
     cr_assert(self->length==5, "cannot find");
      int* test =  get_index_al(self,1);
       cr_assert(*test==9, "should be 9");
       cr_assert(self->capacity==8, "should be 8");
       test = remove_index_al(self,1);
       cr_assert(*test==9, "should be 9");
       cr_assert(self->capacity==8, "should be 8");
         test = remove_index_al(self,1);
         cr_assert(*test==10, "should be 9");
       cr_assert(self->capacity==4, "should be 4");
       test = remove_index_al(self,0);
       test = remove_index_al(self,0);
       cr_assert(self->capacity==4, "should be 4");
       cr_assert(self->length==1, "cannot find 2");
       test = remove_index_al(self,0);
         cr_assert(self->capacity==4, "should be 4");
         test = remove_index_al(self,0);
         cr_assert(test==NULL, "should be no more");
}

Test(al_suite, 2_insertion, .timeout=2, .init=setup, .fini=teardown){
     arraylist_t *self = new_al(sizeof(int));
     int a =0;
     int*b =&a;
     insert_al(self,b);
     a = 10;
     insert_al(self,b);
     insert_al(self,b);
     insert_al(self,b);
     insert_al(self,b);
     insert_al(self,b);
    size_t leng = self->length;
   int* te =  get_index_al(self,1);
    cr_assert(leng==6, "not equal to 6");
    cr_assert(*te==10, "should be 10");
    cr_assert(self->capacity==8, "should be 8");
    insert_al(self,b);
    int index = insert_al(self,b);
    cr_assert(self->capacity==8, "should be 8");
    cr_assert(index==7, "should be 7");
    cr_assert(self->length==8, "should be 8");
    insert_al(self,b);
    cr_assert(self->capacity==16, "should be 8");
    cr_assert(self->length==9, "should be 8");
     int in =  get_data_al(self,b);
     cr_assert(in==1, "should be 1");
}

Test(al_suite, 3_removal, .timeout=2, .init=setup, .fini=teardown){

arraylist_t *test = new_al(sizeof(student_t));
char* name1 = malloc(8);
char* name2 = malloc(8);
char* name3 = malloc(8);
memcpy(name1,"sam",4);
memcpy(name2,"amy",4);
memcpy(name3,"tom",4);
student_t inser = {name1,123,3.0};
student_t inser2 = {name2,123,4.0};
student_t inser3 = {name3,123,2.0};
insert_al(test,&inser);
insert_al(test,&inser2);
insert_al(test,&inser3);
delete_al(test,student_t_free_func);

}
Test(al_suite, 4_insertion, .timeout=2, .init=setup, .fini=teardown){
      arraylist_t *self = new_al(sizeof(int));
      int *save = NULL;
     int a =0;
     int*b =&a;
     insert_al(self,b);
     a = 8;
     insert_al(self,b);
     a = 9;
     insert_al(self,b);
     a = 10;
     insert_al(self,b);
     a = 11;
     insert_al(self,b);
     a = 12;
     insert_al(self,b);
     int no = 1;
     save = & no;
     bool where = remove_data_al(self,save);
      cr_assert(where==false, "should be 1");
      no =0;
      where = remove_data_al(self,save);
       cr_assert(where==true, "should be true");
        int* check =  get_index_al(self,1);
         cr_assert(*check==9, "should be 9");
          size_t leng = self->length;
           cr_assert(leng==5, "should be 5");
           no = 9;
            where = remove_data_al(self,save);
             cr_assert(where==true, "should be true");
             check =  get_index_al(self,2);
              cr_assert(*check==11, "should be 11");
               no = 12;
            where = remove_data_al(self,save);
             cr_assert(where==true, "should be true");
             check =  get_index_al(self,2);
              cr_assert(*check==11, "should be 11");
            leng = self->length;
            size_t cap = self-> capacity;
           cr_assert(leng==3, "should be 3");
           cr_assert(cap==4, "should be 4");
}



