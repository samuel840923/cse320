#include "hw2.h"
#include "mispelling.h"
/* Great filename. */

void processDictionary(FILE* f){
    dict->num_words = 0;
    while(!feof(f))
    {
        //initialize the current word.
        struct dict_word* currWord;
        if((currWord = (struct dict_word*) malloc(sizeof(struct dict_word))) == NULL)
        {
            printf("OUT OF MEMORY.\n");
            return;
        }
        currWord->num_misspellings = 0;
        currWord->misspelled_count = 0;

        //variables
        char word[MAX_SIZE];
        char* wdPtr = word;
        char line[MAX_SIZE];
        char* character = line;
    //    char word_list[MAX_MISSPELLED_WORDS+1][MAX_SIZE];
        int counter = 0;
        int firstWord = 1;

        fgets(line, MAX_SIZE+1, f);
        //if there isn't a space at the end of the line, put one there
        if((line[strlen(line)-2] != ' ' && line[strlen(line)-1] == '\n') || (line[strlen(line)-1] != ' ' && line[strlen(line)-1] != '\n'))
            strcat(line, " ");
        while(*character != '\0')
        {
            if(counter >= MAX_MISSPELLED_WORDS+1)
                break;
            //if the character is a space, add the word in word_list and make word NULL.
            if(*character == ' ')
            {
                *wdPtr = '\0';
                wdPtr = word;
                if(firstWord)
                {
                    addWord(currWord, wdPtr);
                    dict->num_words++;

                    firstWord = 0;
                }
                else
                {
                    struct misspelled_word* currMisspelling = NULL;
                    if((currMisspelling = malloc(sizeof(struct misspelled_word))) == NULL)
                    {
                        printf("ERROR: OUT OF MEMORY.");
                        return;
                    }

                    addMisspelledWord(currMisspelling, currWord, wdPtr);

                }
            }
            //if the character isn't a space or a new line, add the character to word.
            else if(*character != '\n')
                *(wdPtr++) = *character;
            character++;

        }

    }

}

void addWord(struct dict_word* dWord, char* word){
    //setting up dWord fields
    dWord->misspelled_count = 0;
    dWord->num_misspellings = 0;
    dWord->next = dict->word_list;
    strcpy(dWord->word, word);
    dict->word_list = dWord;
}

void addMisspelledWord(struct misspelled_word* misspelledWord, struct dict_word* correctWord, char* word){
    //setting up misspelledWord fields
    strcpy(misspelledWord->word, word);
    misspelledWord->misspelled = 0;
    misspelledWord->correct_word = correctWord;
    misspelledWord->next = m_list;
    (correctWord->misspelled)[correctWord->num_misspellings++] = misspelledWord;
    m_list = misspelledWord;
}

void freeWords(struct dict_word* currWord){
    if(currWord != NULL)
    {
        freeWords(currWord->next);
        //free word

        free(currWord);
    }
}
void freeMiss(struct misspelled_word* currWord){
    if(currWord != NULL)
    {
        freeMiss(currWord->next);


        //free word

        free(currWord);
    }
}



void printWords(struct dict_word* currWord, FILE* f){
    if(currWord != NULL)
    {
        printWords(currWord->next, f);

        int i;

        fprintf(f, "%s ", currWord->word);

        for(i = 0; i<currWord->num_misspellings; i++)
        {
            fprintf(f, "%s ",((currWord->misspelled)[i])->word);

        }
         fprintf(f, "\n");


    }
}

void processWord(char* inputWord,int n, int* check){
    if(foundMisspelledMatch(inputWord))
        return;
    if(foundDictMatch(inputWord))
        return;
    else
    {
        if(n>0){
          *check =1;
            struct dict_word* newWord;
           // int counter = 0;
             dict->num_words++;
            if((newWord = (struct dict_word*) malloc(sizeof(struct dict_word))) == NULL)
            {
                printf("ERROR: OUT OF MEMORY.\n");
                return;
            }

            addWord(newWord, inputWord);
               dict->word_list = newWord;
                int counter=0;
                int numMisspellings=n;
               char** setOfMiss = gentypos(n, inputWord);
                while(numMisspellings > 0)
                {
                    struct misspelled_word* newMWord;

                    if((newMWord = (struct misspelled_word*) malloc(sizeof(struct misspelled_word))) == NULL)
                    {
                        printf("ERROR: OUT OF MEMORY.");
                        return;
                    }
                    char* newMiss= *(setOfMiss+counter);
                    addMisspelledWord(newMWord, newWord, newMiss);
                    counter++;
                    numMisspellings--;
                }
                for(int i=0;i<n;i++){
                    free(setOfMiss[i]);
                }
                free(setOfMiss);
    }

}
}

bool foundMisspelledMatch(char* inputWord){
    struct misspelled_word* listPtr = m_list;
    while(listPtr != NULL)
    {
        if(strcasecmp(inputWord, listPtr->word) == 0)
        {

            listPtr->misspelled = 1;
            listPtr->correct_word->misspelled_count++;
            return true;
        }
        listPtr = listPtr->next;
    }
    return false;
}

bool foundDictMatch(char* inputWord){
    struct dict_word* listPtr = dict->word_list;
    while(listPtr != NULL)
    {
        if(strcasecmp(inputWord, listPtr->word) == 0)
            return true;
        listPtr = listPtr->next;
    }
    return false;
}
char* foundMisspelled(char* inputWord){
    struct misspelled_word* listPtr = m_list;
    while(listPtr != NULL)
    {
        if(strcasecmp(inputWord, listPtr->word) == 0)
        {
            strcpy(inputWord, listPtr->correct_word->word);
            return  inputWord;
        }
        listPtr = listPtr->next;
    }
    return NULL;
}
char* findDict(char* inputWord){
    struct dict_word* listPtr = dict->word_list;
    while(listPtr != NULL)
    {
        if(strcasecmp(inputWord, listPtr->word) == 0)
            return inputWord;
        listPtr = listPtr->next;
    }
    return NULL;
}
int actualWord(char* c){

    int size = strlen(c);
    for(int i=size-1;i>0;i--){
        if((c[i]>='a'&&c[i]<='z')||(c[i]>='A'&&c[i]<='Z')){
            return i;
        }
    }
    return -1;
}
int checkSlash(char*c){
     int size = strlen(c);
    for(int i=size-1;i>0;i--){
        if(c[i]=='/'){
            return i;
        }
    }
    return -1;
}
void printStat(struct dictionary* currWord){
fprintf(stderr,"%s%d\n","Total number of words in dictionary: ",currWord->num_words);
int dicS = countDictSize(currWord->word_list);
dicS += sizeof(*currWord);
fprintf(stderr, "%s%d\n", "Size of dictionary (in bytes): ",dicS);
fprintf(stderr,"%s%d\n","Size of misspelled word list (in bytes): ",countMiss(currWord->word_list));
fprintf(stderr,"%s%d\n","Total number of misspelled words: ",totalMiss(currWord->word_list));
fprintf(stderr,"%s\n","Top 3 misspelled words:");

struct dict_word* first =NULL;
struct dict_word* second = NULL;
struct dict_word* third=NULL;
struct dict_word* curr = currWord->word_list;

int top1 =-1;
int top2 =-1;
int top3 =-1;
while(curr!=NULL){
int currentMax = curr->misspelled_count;

if(currentMax>top1){
    top3 = top2;
    top2 = top1;

    third = second;
    second = first;

    first = curr;
    top1 = currentMax;
}

else if(currentMax>top2){
    top3 = top2;

    third = second;

    second = curr;
    top2 = currentMax;
}

else if(currentMax>top3){
    third = curr;
    top3 = currentMax;

}

curr = curr->next;
}
if(first!=NULL){
fprintf(stderr,"%s (%d times): ",first->word,first->misspelled_count);
int size = first->num_misspellings;
int f =1;
for (int i=0;i<size;i++){

  if((first->misspelled)[i]->misspelled==1){
    if(f==0){
        fprintf(stderr,"%s",", ");
    }
    f=0;
    fprintf(stderr,"%s",(first->misspelled)[i]->word);
    }
}
fprintf(stderr,"\n");
}

if(second!=NULL){
  fprintf(stderr,"%s (%d times): ",second->word,second->misspelled_count);
int size = second->num_misspellings;
int f =1;
for (int i=0;i<size;i++){

  if((second->misspelled)[i]->misspelled==1){
    if(f==0){
        fprintf(stderr,"%s",", ");
    }
    f=0;
    fprintf(stderr,"%s",(second->misspelled)[i]->word);
    }
}
fprintf(stderr,"\n");
}
if(third!=NULL){
   fprintf(stderr,"%s (%d times): ",third->word,third->misspelled_count);
int size = third->num_misspellings;
int f =1;
for (int i=0;i<size;i++){

  if((third->misspelled)[i]->misspelled==1){
    if(f==0){
        fprintf(stderr,"%s",", ");
    }
    f=0;
    fprintf(stderr,"%s",(third->misspelled)[i]->word);
    }
}
}



}
int countDictSize(struct dict_word* curr){
    int i=0;
    while (curr!=NULL){
        i=i+sizeof(*curr);
        curr = curr->next;
    }
    return i;
}
int countMiss(struct dict_word* curr){

 int all=0;
    while (curr!=NULL){
        int size = curr->num_misspellings;
        for(int i=0;i<size;i++){
           all+= sizeof(*(curr->misspelled[i]));
        }
 curr = curr->next;
    }

    return all;
}
int totalMiss(struct dict_word* curr){
    int total =0;
     while (curr!=NULL){
        total += curr->misspelled_count;
      curr = curr->next;
    }
    return total;
}
int frontCheck(char* c){
    int size = strlen(c);
    for (int i=0;i<size;i++){
        if((c[i]>='a'&&c[i]<='z')||(c[i]>='A'&&c[i]<='Z'))
            return i;
    }
    return -1;
}


