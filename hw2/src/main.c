#include "hw2.h"
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>

int main(int argc, char *argv[]){


    DEFAULT_INPUT = stdin;
    DEFAULT_OUTPUT = stdout;

    //create dictionary
    if((dict = (struct dictionary*) calloc(1,sizeof(struct dictionary))) == NULL)
    {

        return EXIT_FAILURE;
    }

   if((m_list = (struct misspelled_word*) calloc(1,sizeof(struct misspelled_word))) == NULL)
    {

        return EXIT_FAILURE;
    }

    int dicChange=0;

    struct Args args;
    // Set struct default values
    args.d = false;
    args.i = false;
    args.o = false;
    char DEFAULT_DICT_FILE[]= "rsrc/dictionary.txt";
    strcpy(args.dictFile, DEFAULT_DICT_FILE);
    // Make a loop index
    int i;
    char line[MAX_SIZE];
    //Declare Files
    FILE* dFile;
    FILE* iFile = DEFAULT_INPUT;
    FILE* oFile = DEFAULT_OUTPUT;


 char flag ='\0';
    int A =-1;
    opterr =0;
    while((flag=getopt(argc,argv,"Ah012345i:o:d:"))!=-1){
        switch(flag){
            case 'A': A=0; break;
            case '0': A = (10*A)+(flag-'0'); break;
            case '1': A = (10*A)+(flag-'0'); break;
            case '2': A = (10*A)+(flag-'0'); break;
            case '3': A = (10*A)+(flag-'0'); break;
            case '4': A = (10*A)+(flag-'0'); break;
            case '5': A = (10*A)+(flag-'0'); break;
            case 'h': USAGE(EXIT_SUCCESS); return EXIT_SUCCESS;
            case '?':  return EXIT_FAILURE;
      }
    }




    char opt = '\0';

    for(i = 1; i< argc; i++)
    {
        char* currArg = argv[i];
        //there's a flag
        if(opt != '\0')
        {
            if(opt == 'd')
            {
                strcpy(args.dictFile, currArg);
                args.d = true;
            }
            if(opt == 'i')
            {
                strcpy(args.input, currArg);
                args.i = true;
                iFile = fopen(currArg, "r");
            }
            if(opt == 'o')
            {
                strcpy(args.output, currArg);
                args.o = true;
                oFile = fopen(currArg, "w");
            }

            opt = '\0';
        }
        else
        {
            if(strcmp(currArg, "-d") == 0)
                opt = 'd';
            else if(strcmp(currArg, "-i") == 0)
                opt = 'i';
            else if(strcmp(currArg, "-o") == 0)
                opt = 'o';
            else if(*(currArg) != '-'){

                return EXIT_FAILURE;
            }
        }
    }

     if(A<-1||A>5){
        USAGE(EXIT_FAILURE);
       return EXIT_FAILURE;
     }
    dFile = fopen(args.dictFile, "r");

    if(iFile == NULL && args.i == true)
    {
        USAGE(EXIT_FAILURE);
        return EXIT_FAILURE;
    }
    if(dFile == NULL)
    {
          USAGE(EXIT_FAILURE);
       return EXIT_FAILURE;
    }
    else
    {
        processDictionary(dFile);

    }

    //strcpy(line,"\n--------INPUT FILE WORDS--------\n");
   // fwrite(line, strlen(line)+1, 1, oFile);

    while(!feof(iFile))
    {
        char word[MAX_SIZE];
        char* wdPtr = word;
        //char line[MAX_SIZE];
        char* character = line;

        fgets(line, MAX_SIZE+1, iFile);

        //if there isn't a space or newline at the end of the line, put one there
        if((line[strlen(line)-1] != ' ') && (line[strlen(line)-1] != '\n'))
            strcat(line, " ");
        //replaces spaces within a line with new lines
        while(*character != '\0')
        {
            if(*character ==' '||*character=='\n')
            {

                /*char* punct = wdPtr-1;
                    printf("char:%c",punct);
                while(!((*punct>='a' && *punct<='z') || (*punct>='A' && *punct<='Z')))
                {
                    punct--;
                }
                punct++;
                printf("%d", strlen(wdPtr)-strlen(punct));
                */

                *wdPtr = '\0';
                wdPtr = word;
                int length = strlen(wdPtr)-1;
                int actual  = actualWord(wdPtr);
                int front = frontCheck(wdPtr);
                if(actual!=-1){
                char WordAct[MAX_SIZE] ;
                int counter =0;
                for(int i=front;i<=actual;i++){
                    WordAct[counter]= wdPtr[i];
                    counter++;
                }
                int offset = length-actual;
                 WordAct[counter]='\0';

                processWord(WordAct,A,&dicChange);

                for(int i=0;i<front;i++){
                     fprintf(oFile,"%c",wdPtr[i]);
                }

                char* bePrint= foundMisspelled(WordAct);
                if(bePrint==NULL){
                  bePrint =  findDict(WordAct);
                    if(bePrint==NULL){
                        bePrint = WordAct;
                    }
                }

                fprintf(oFile,"%s",bePrint);
                for(int i=offset;i>0;i--){
                    fprintf(oFile,"%c",*(character-i));
                }
                 fprintf(oFile,"%c",*character);

                 }
            else{
                 fprintf(oFile, "%s",wdPtr );
                 fprintf(oFile,"%c",*character);
            }

                //fprintf(oFile, "%s",bePrint);
               // fwrite(wdPtr, strlen(wdPtr)+1, 1, oFile);
            }
            else
            {
                *(wdPtr) = tolower(*character);

                wdPtr++;
            }
            character++;
        }

        if(iFile == stdin)
            break;
        char check =getc(iFile);
        if(check==-1)
            break;
        else
            ungetc(check,iFile);

    }

   // strcpy(line, "\n--------DICTIONARY WORDS--------\n");

   if(dicChange==1){
    char DICT_FILE[MAX_SIZE];
    char NEW_DICT[MAX_SIZE];
    int checkslah = checkSlash(args.dictFile);
    int length = strlen(args.dictFile);

    DICT_FILE[0]='n';
    DICT_FILE[1]='e';
    DICT_FILE[2]='w';
    DICT_FILE[3]='_';
    int j=4;
    for (int i=checkslah+1;i<length;i++){
        DICT_FILE[j] = *(args.dictFile+i);
        j++;
    }
     DICT_FILE[j]='\0';
     int x=0;
     for(int i=0;i<=checkslah;i++){
        NEW_DICT[i]=*(args.dictFile+i);
        x++;
     }
     int start =x;
     int size = strlen(DICT_FILE);
     for (int i=0;i<size;i++){
         NEW_DICT[start+i]=DICT_FILE[i];
         x++;
     }
     NEW_DICT[x]='\0';
     char* newD = NEW_DICT;


    FILE* newDict = fopen(newD, "w");
  printWords(dict->word_list , newDict);
  fclose(newDict);
   }
   printStat(dict);
    freeWords(dict->word_list);

    //printf("\n--------FREED WORDS--------\n");
    freeMiss(m_list);

    //free dictionary
    free(dict);
    //free m_list


    fclose(dFile);
    fclose(iFile);
    fclose(oFile);
    return EXIT_SUCCESS;
}
