#include "debug.h"
#include "arraylist.h"
#include "foreach.h"
#include <unistd.h>
void *testcode(void* data);
void *testcode_read(void* data);
void *testcode_remove(void* data);
void *testread_each1(void* data);
int numbers(void* data);
void *testread_each(void* data);
typedef struct {
    char* name;
    int32_t id;
    double gpa;
}student_t;
void *testcode_write(void* data);


void student_t_free_func(void *argptr){
    student_t* ptr = (student_t*) argptr;
    if(ptr != NULL)
        free(ptr->name);
    else
        printf("%s\n", "Pointer was NULL");
}

void *delet_test(void* data){
	arraylist_t *hi = data;
	delete_al(hi,student_t_free_func);
	return NULL;
}

void *apply_test(void* data);


int main(int argc, char *argv[]){
	pthread_t tested[2000];
	//pthread_t tested2[2000];
arraylist_t *test = new_al(sizeof(student_t));

for(int i=0;i<100;i++){
	pthread_create(&tested[i],NULL,testcode_read,test);

}
for(int i=100;i<200;i++){
pthread_create(&tested[i],NULL,testcode_write,test);

}
for(int i=200;i<250;i++){
	pthread_create(&tested[i],NULL,testcode_remove,test);


}
for(int i=0;i<250;i++){
	pthread_join(tested[i],NULL);

}
size_t len = test->length;

printf("end\n");
printf("the length is %zu\n",len);


/*
for(int i=0;i<100;i++){
	char* na = malloc(8);
	memcpy(na,"sam",3);
	student_t inser = {na,i,3.0};
	insert_al(test,&inser);
}
pthread_t tid,tid1,tid2;

pthread_create(&tid,NULL,testread_each1,test);
pthread_create(&tid1,NULL,testread_each,test);
pthread_create(&tid2,NULL,apply_test,test);

//pthread_create(&tid4,NULL,testcode_remove,test);
//pthread_create(&tid5,NULL,apply_test,test);


pthread_join(tid,NULL);
pthread_join(tid1,NULL);
pthread_join(tid2,NULL);
//pthread_join(tid3,NULL);
//pthread_join(tid4,NULL);
//pthread_join(tid5,NULL);
*/
/*
size_t len = test->length;
for(int i=0;i<len;i++){
	student_t *get = get_index_al(test,i);
	printf("the id %d\n", get->id);
}
*/
}
void *testread_each(void* data){
	arraylist_t *hi = data;
	foreach(student_t, value, hi){
		size_t test =value->id;
		printf("the index from 1: %zu\n",test);

	}


	return NULL;
}
void *testread_each1(void* data){
	arraylist_t *hi = data;
	foreach(student_t, value, hi){
		size_t test =value->id;
		printf("the index from 2: %zu\n",test);
		if(test==50)foreach_break;
	}
	return NULL;
}
void *testcode(void* data){
	arraylist_t *hi = data;
	for(int i=0;i<10;i++){
		student_t inser = {"sam",i,3.0};
	insert_al(hi,&inser);
}
return NULL;
}
void *testcode_write(void* data){
	arraylist_t *hi = data;
	student_t inser = {"sam",9,3.0};
	insert_al(hi,&inser);


return NULL;
}
void *testcode_read(void* data){
	arraylist_t *hi = data;
	student_t inser = {"sam",9,3.0};
	int te = get_data_al(hi,&inser);
	printf("the index is : %d\n",te);


return NULL;
}
int numbers(void* data){
	((student_t*)data)->id*=3;
	return 0;
}
void *testcode_remove(void* data){
	arraylist_t *hi = data;
	remove_index_al(hi,0);
	return NULL;

}
void *apply_test(void* data){
	arraylist_t *hi = data;
	apply(hi,numbers);
	return NULL;
}

