#include "debug.h"
#include "arraylist.h"
#include "foreach.h"
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


int main(int argc, char *argv[]){
	//pthread_t tested[2000];
arraylist_t *test = new_al(sizeof(student_t));
/*
for(int i=0;i<1000;i++){
	pthread_create(&tested[i],NULL,testcode_write,test);
}
for(int i=1000;i<2000;i++){
	pthread_create(&tested[i],NULL,testcode_write,test2);
}
for(int i=0;i<2000;i++){
	pthread_join(tested[i],NULL);
}
*/
for(int i=0;i<3000;i++){
	student_t inser = {"sam",i,3.0};
	insert_al(test,&inser);
}
pthread_t tid,tid1,tid2,tid3,tid4,tid5;

pthread_create(&tid,NULL,testread_each,test);
pthread_create(&tid1,NULL,testcode_remove,test);

pthread_create(&tid2,NULL,testread_each1,test);
pthread_create(&tid3,NULL,testcode_remove,test);
pthread_create(&tid4,NULL,testcode_remove,test);
pthread_create(&tid5,NULL,testcode_remove,test);


pthread_join(tid,NULL);
pthread_join(tid1,NULL);

pthread_join(tid2,NULL);
pthread_join(tid3,NULL);
pthread_join(tid4,NULL);
pthread_join(tid5,NULL);

}
void *testread_each(void* data){
	arraylist_t *hi = data;
	foreach(student_t, value, hi){
		size_t test =foreach_index();
		printf("the index from 1: %zu\n",test);
	}


	return NULL;
}
void *testread_each1(void* data){
	arraylist_t *hi = data;
	foreach(student_t, value, hi){
		size_t test =foreach_index();
		printf("the index from 2: %zu\n",test);

	}
	return NULL;
}
void *testcode(void* data){
	student_t inser = {"sam",123,3.0};
	arraylist_t *hi = data;
	for(int i=0;i<40;i++)
	insert_al(hi,&inser);
return NULL;
}
void *testcode_write(void* data){
	student_t inser = {"leon",321,4.0};
	arraylist_t *hi = data;
	insert_al(hi,&inser);
return NULL;
}
void *testcode_read(void* data){
	arraylist_t *hi = data;
	for(int i=0;i<100;i++){
	student_t*te = get_index_al(hi,i);
	if(te != NULL){
		printf("the index is: %d word: %s\n",i,te->name);
	}
}
return NULL;
}
int numbers(void* data){
	((student_t*)data)->gpa++;
	return 0;
}
void *testcode_remove(void* data){
	arraylist_t *hi = data;
	for(int i=0;i<3000;i++)
	remove_index_al(hi,0);
	return NULL;
}
