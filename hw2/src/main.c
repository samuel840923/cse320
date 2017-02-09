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
        printf("ERROR: OUT OF MEMORY.\n");
        return EXIT_FAILURE;
    }

   if((m_list = (struct misspelled_word*) calloc(1,sizeof(struct misspelled_word))) == NULL)
    {
        printf("ERROR: OUT OF MEMORY.\n");
        return EXIT_FAILURE;
    }



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
            if(strcmp(currArg, "-i") == 0)
                opt = 'i';
            if(strcmp(currArg, "-o") == 0)
                opt = 'o';

        }
    }
    char flag ='\0';
    int A =-1;
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
            case '?': return EXIT_FAILURE;
      }
    }
     if(A<-1||A>5){
       return EXIT_FAILURE;
     }
    dFile = fopen(args.dictFile, "r");

    if(iFile == NULL && args.i == true)
    {
        printf("Unable to open: %s.\n", args.input);
        return EXIT_FAILURE;
    }
    if(dFile == NULL)
    {
        printf("Unable to open: %s.\n", args.dictFile);
         return EXIT_FAILURE;
    }
    else
    {
        processDictionary(dFile);

    }

    strcpy(line,"\n--------INPUT FILE WORDS--------\n");
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

                processWord(wdPtr,3);

                char* bePrint= foundMisspelled(wdPtr);
                if(bePrint==NULL){
                  bePrint =  findDict(wdPtr);
                    if(bePrint==NULL){
                        bePrint = wdPtr;
                    }
                }

                printf("%s",bePrint);
                while(*character != '\0'&&(!((*character>='a' && *character<='z') || (*character>='A' && *character<='Z')))){
                    printf("%c",*character);
                    character++;
                }
                character--;
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

    strcpy(line, "\n--------DICTIONARY WORDS--------\n");
   // fwrite(line, strlen(line)+1, 1, oFile);
  //  printWords(dict->word_list , oFile);
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
