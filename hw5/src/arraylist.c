#include "arraylist.h"
#include <errno.h>

/**
 * @visibility HIDDEN FROM USER
 * @return     true on success, false on failure
 */

static bool resize_al(arraylist_t* self){
    bool ret = false;
    size_t curr_cap = self->capacity;
    size_t curr_length = self->length;
    size_t item_size = self->item_size;
    void *curr_base = self->base;
    if(curr_cap == curr_length){
        self->base = realloc(curr_base,curr_cap*2*item_size);
        curr_base = self->base;
         curr_cap = curr_cap*2;
         self->capacity = curr_cap;
         size_t move = curr_length*item_size;
         curr_base = (char*)curr_base+move;
         size_t tobeclear = curr_cap - curr_length;
         tobeclear = tobeclear*item_size;
         memset(curr_base,0,tobeclear);
         ret = true;
    }
    else if((curr_length == (curr_cap/2) - 1)&&((curr_cap/2)>=INIT_SZ)){
        self->base = realloc(curr_base,curr_cap*item_size/2);
        curr_cap = curr_cap/2;
        self->capacity = curr_cap;
        ret = true;
    }
    return ret;
}

arraylist_t *new_al(size_t item_size){
    arraylist_t *ret = (arraylist_t*) malloc(sizeof(arraylist_t));
    ret -> capacity = INIT_SZ;
    ret -> length =0;
    ret -> item_size = item_size;
    ret -> base = calloc(INIT_SZ,item_size);
    ret -> readcnt=0;
    ret -> getcnt=0;
    pthread_mutex_init(&(ret->mutex), NULL);
    pthread_mutex_init(&(ret->foreach_mutex), NULL);
    pthread_mutex_init(&(ret->read_mutex), NULL);
    pthread_mutex_init(&(ret->get_mutex), NULL);
    return ret;
}

size_t insert_al(arraylist_t *self, void* data){
    if(self == NULL){
       errno = EINVAL;
        return UINT_MAX;
    }
    pthread_mutex_lock(&(self->mutex));
    size_t curr_length = self->length;
    size_t curr_cap = self->capacity;
    if(curr_cap==0){
        pthread_mutex_unlock(&(self->mutex));
        errno = EINVAL;
        return UINT_MAX;
    }

    if(curr_cap==curr_length)
        resize_al(self);

    size_t item_size = self->item_size;
    size_t move = item_size*curr_length;
    void *nowbase = self->base;
    nowbase = (char*)nowbase+move;
    memcpy(nowbase,data,item_size);
    self->length++;
    pthread_mutex_unlock(&(self->mutex));
return curr_length;
}


size_t get_data_al(arraylist_t *self, void *data){
    if(self==NULL){
        errno = EINVAL;
        return UINT_MAX ;
    }
    pthread_mutex_lock(&(self->get_mutex));
    size_t cap = self->capacity;
    if(cap==0){
        pthread_mutex_unlock(&(self->get_mutex));
         errno = EINVAL;
        return UINT_MAX ;
    }
    self->getcnt++;
    if(self->getcnt==1)
        pthread_mutex_lock(&(self->mutex));
    pthread_mutex_unlock(&(self->get_mutex));

    if(data==NULL){
        pthread_mutex_lock(&(self->get_mutex));
         self->getcnt--;
         if(self->getcnt==0)
            pthread_mutex_unlock(&(self->mutex));
         pthread_mutex_unlock(&(self->get_mutex));
        return 0;
    }
    int index = getindex(self,data);
    if(index==-1){
        pthread_mutex_lock(&(self->get_mutex));
         self->getcnt--;
         if(self->getcnt==0)
            pthread_mutex_unlock(&(self->mutex));
         pthread_mutex_unlock(&(self->get_mutex));
        errno = EINVAL;
        return UINT_MAX ;
    }
    pthread_mutex_lock(&(self->get_mutex));
         self->getcnt--;
         if(self->getcnt==0)
            pthread_mutex_unlock(&(self->mutex));
        pthread_mutex_unlock(&(self->get_mutex));

    return index;


}

void *get_index_al(arraylist_t *self, size_t index){
    if(self==NULL){
        errno = EINVAL;
        return NULL;
    }
    pthread_mutex_lock(&(self->get_mutex));
    self->getcnt++;
    if(self->getcnt==1)
         pthread_mutex_lock(&(self->mutex));
     pthread_mutex_unlock(&(self->get_mutex));
    size_t length = self->length;
    void *baseArray = self -> base;
     size_t elem_size = self->item_size;
     size_t cap = self->capacity;
    if(length==0||cap==0){
        errno = ENXIO;
         pthread_mutex_lock(&(self->get_mutex));
             self->getcnt--;
                if(self->getcnt==0)
         pthread_mutex_unlock(&(self->mutex));
     pthread_mutex_unlock(&(self->get_mutex));
        return NULL;
    }
    if(length<=index) {
    size_t move = elem_size*(length-1);
    baseArray = (char*)baseArray+move;
    void * orig_data = malloc(elem_size);
    memcpy(orig_data,baseArray,elem_size);
     pthread_mutex_lock(&(self->get_mutex));
             self->getcnt--;
                if(self->getcnt==0)
         pthread_mutex_unlock(&(self->mutex));
     pthread_mutex_unlock(&(self->get_mutex));
    return orig_data;
    }

    size_t move = elem_size*index;
    baseArray = (char*)baseArray+move;
    void * orig_data = malloc(elem_size);
    memcpy(orig_data,baseArray,elem_size);
     pthread_mutex_lock(&(self->get_mutex));
             self->getcnt--;
                if(self->getcnt==0)
         pthread_mutex_unlock(&(self->mutex));
     pthread_mutex_unlock(&(self->get_mutex));
    return orig_data;
}


bool remove_data_al(arraylist_t *self, void *data){
    if(self==NULL){
     errno = EFAULT ;
         return false;
    }
    pthread_mutex_lock(&(self->foreach_mutex));
     pthread_mutex_lock(&(self->mutex));
    size_t length = self->length;
    size_t elem_size = self->item_size;
    size_t cap = self->capacity;
    void* base = self->base;
    if(length==0||cap==0){
     pthread_mutex_unlock(&(self->mutex));
     pthread_mutex_unlock(&(self->foreach_mutex));
      errno = ENXIO;
        return false;
            }
   if(data==NULL){
            void *removed = malloc(elem_size);
            memcpy(removed,base,elem_size);
            shiftleft(self,0);
            self->length--;
             if(self->length == (cap/2) - 1)
                resize_al(self);
             pthread_mutex_unlock(&(self->mutex));
              pthread_mutex_unlock(&(self->foreach_mutex));
            return true;
   }
   int index = getindex(self,data);
   if(index==-1){
    pthread_mutex_unlock(&(self->mutex));
     pthread_mutex_unlock(&(self->foreach_mutex));
    return false;
   }

    size_t move = index*elem_size;
    void *removed = malloc(elem_size);
    memcpy(removed,((char*)base+move),elem_size);
    shiftleft(self,index);
    self->length--;
    if(self->length == (cap/2) - 1)
        resize_al(self);

    pthread_mutex_unlock(&(self->mutex));
    pthread_mutex_unlock(&(self->foreach_mutex));
    return true;
}

void *remove_index_al(arraylist_t *self, size_t index){
    if(self==NULL){
         errno = EFAULT;
         return false;
    }
     pthread_mutex_lock(&(self->foreach_mutex));
     pthread_mutex_lock(&(self->mutex));

    size_t len = self->length;
    size_t cap = self->capacity;
    size_t elem_size = self->item_size;
    void* base = self->base;
    if(len==0||cap==0){
        pthread_mutex_unlock(&(self->mutex));
         pthread_mutex_unlock(&(self->foreach_mutex));

        errno = ENXIO;
        return NULL;
    }
    if(index>=len){
            void *removed = malloc(elem_size);
            size_t move = elem_size*(len-1);
            memcpy(removed,(char*)base+move,elem_size);
            shiftleft(self,index);
            self->length--;
             if(self->length == (cap/2) - 1)
                  resize_al(self);

           pthread_mutex_unlock(&(self->mutex));
           pthread_mutex_unlock(&(self->foreach_mutex));
            return removed;
    }

            void *removed = malloc(elem_size);
            size_t move = elem_size*(index);
            memcpy(removed,(char*)base+move,elem_size);
            shiftleft(self,index);
            self->length--;
             if(self->length == (cap/2) - 1)
                  resize_al(self);
       pthread_mutex_unlock(&(self->mutex));
       pthread_mutex_unlock(&(self->foreach_mutex));

            return removed;
}

void delete_al(arraylist_t *self, void (*free_item_func)(void*)){
if(self==NULL){
    errno = EFAULT;

    return;
}

pthread_mutex_lock(&(self->foreach_mutex));
pthread_mutex_lock(&(self->mutex));

if(self->capacity ==0){
    errno = EFAULT;
    pthread_mutex_unlock(&(self->mutex));
    pthread_mutex_unlock(&(self->foreach_mutex));
    return;
}

if(free_item_func==NULL){
    self->capacity =0;
    free(self->base);
     pthread_mutex_unlock(&(self->mutex));
     pthread_mutex_unlock(&(self->foreach_mutex));
    return;
}
size_t leng = self->length;
void *base = self->base;
size_t elem_size = self->item_size;
for(int i=0;i<leng;i++){
    size_t move = elem_size*i;
    void *freebase = (char*)base+move;
    free_item_func(freebase);
}
free(self->base);
self->capacity =0;
self->length=0;
pthread_mutex_unlock(&(self->mutex));
pthread_mutex_unlock(&(self->foreach_mutex));
//pthread_mutex_destroy(&(self->mutex));
//pthread_mutex_destroy(&(self->foreach_mutex));
}

int getindex(arraylist_t *self, void *data){
    int length = self->length;
    void *baseA = self -> base;
    size_t item_size = self->item_size;
    for(int i=0;i<length;i++){
        size_t move = item_size*i;
        if(memcmp((char*)baseA+move,data,item_size)==0){
            return i;
        }
    }

    return -1;
}
int shiftleft (arraylist_t *self, int index){
    int length = self->length;
    size_t elem_size = self->item_size;
    void* baseA = self->base;

    for(int i=index;i<length-1;i++){
        size_t now = i*elem_size;
        size_t next = (i+1)*elem_size;
        memmove((char*)baseA+now,(char*)baseA+next,elem_size);
    }
    size_t last = (length-1)*elem_size;
    baseA = (char*)baseA+last;
    memset(baseA,0,elem_size);
    return 0;
}
