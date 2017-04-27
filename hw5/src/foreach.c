#include "debug.h"
#include "arraylist.h"
#include "foreach.h"
static pthread_key_t key = -1;
static pthread_once_t key_once = PTHREAD_ONCE_INIT;
static void make_key()
{
    (void) pthread_key_create(&key, free);
}

void *foreach_init(arraylist_t *self){
    if(self ==NULL){
        return NULL;
    }
     pthread_mutex_lock(&(self->foreach_mutex));
    if(self->capacity==0){
         pthread_mutex_unlock(&(self->foreach_mutex));
        return NULL;
    }
     pthread_mutex_unlock(&(self->foreach_mutex));
    void *ret = NULL;
    ret = get_index_al (self,0);
    if(ret == NULL){
        return NULL;
    }
    storage_t *size = malloc(sizeof(storage_t));
    size->index = 0;
    size->mutex = &(self->foreach_mutex);
    size->check = self;
    pthread_once(&key_once, make_key);
    pthread_setspecific(key, size);

    pthread_mutex_lock(&(self->read_mutex));
    self->readcnt++;
    if(self->readcnt==1){
       pthread_mutex_lock(&(self->foreach_mutex));
    }
    pthread_mutex_unlock(&(self->read_mutex));
    return ret;
}

void *foreach_next(arraylist_t *self, void* data){
     if(self ==NULL){
        return NULL;
    }
   void *ret = NULL;

   size_t the_index =  get_data_al (self, data);
   if(the_index == UINT_MAX){
    pthread_mutex_lock(&(self->read_mutex));
        self->readcnt--;
        if(self->readcnt==0)
             pthread_mutex_unlock(&(self->foreach_mutex));
        pthread_mutex_unlock(&(self->read_mutex));
       return NULL;
    }
pthread_mutex_lock(&(self->mutex));
   size_t length = self->length;
pthread_mutex_unlock(&(self->mutex));
   the_index++;
    if(the_index>=length){
        pthread_mutex_lock(&(self->read_mutex));
        self->readcnt--;
        if(self->readcnt==0)
             pthread_mutex_unlock(&(self->foreach_mutex));
        pthread_mutex_unlock(&(self->read_mutex));
        //pthread_key_delete(key);
        return NULL;
    }
    ret = get_index_al(self,the_index);

    if(ret == NULL){
        pthread_mutex_lock(&(self->read_mutex));
        self->readcnt--;
        if(self->readcnt==0)
             pthread_mutex_unlock(&(self->foreach_mutex));
        pthread_mutex_unlock(&(self->read_mutex));
        return ret;
    }
    storage_t *size =  pthread_getspecific(key);
    if(size==NULL){
       return NULL;
    }
    else{
    size->index = the_index;
    //pthread_setspecific(key,size);
    }
   return ret;
}

size_t foreach_index(){
    size_t ret = 0;
    storage_t *size =  pthread_getspecific(key);
    if(size!=NULL){
    ret = size->index;
    return ret;
    }
    else
        return UINT_MAX;
}

bool foreach_break_f(){
    bool ret = true;
    storage_t *size =  pthread_getspecific(key);
    arraylist_t *array = size->check;
    pthread_mutex_lock(&(array->read_mutex));
    array->readcnt--;
    if(size!=NULL&&array->readcnt==0){
     pthread_mutex_unlock(size->mutex);
    }
     pthread_mutex_unlock(&(array->read_mutex));
    return ret;
}

int32_t apply(arraylist_t *self, int32_t (*application)(void*)){
    int32_t ret = 0;
    size_t elem_size = self->item_size;
    pthread_mutex_lock(&(self->mutex));
    if(self->capacity==0){
        pthread_mutex_unlock(&(self->mutex));
         return -1;
    }
    pthread_mutex_unlock(&(self->mutex));

    void* test = malloc(elem_size);
        foreach(void, value, self){
           if( application(test)==0){
            pthread_mutex_lock(&(self->mutex));
            application(value);
            void* base = self->base;
            size_t index = foreach_index();
             if(self->capacity==0)
                return -1;
            size_t move = index*elem_size;
            base = (char*)base + move;
            memcpy(base,value,elem_size);
            pthread_mutex_unlock(&(self->mutex));
           }
        }
        free(test);
    return ret;
}
