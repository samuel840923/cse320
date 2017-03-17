#include "sfish.h"
#include "debug.h"

/*
 * As in previous hws the main function must be in its own file!
 */

int main(int argc, char const *argv[], char* envp[]){
    /* DO NOT MODIFY THIS. If you do you will get a ZERO. */
    rl_catch_signals = 0;
    /* This is disable readline's default signal handlers, since you are going to install your own.*/
    char *cmd; char* prompt= modif_prompt(NULL);
    while((cmd = readline(prompt)) != NULL) {
        int arg_c = getArgc(cmd);
        char **arg_v =   parse_command(cmd,arg_c);
       if(strcmp(arg_v[0],"help")==0){ //help
        print_help(arg_c,arg_v);
       }
       else if(strcmp(arg_v[0],"exit")==0){
                      free(cmd);
                      free(prompt);
                      exit(EXIT_SUCCESS);
           }
       else if(strcmp(arg_v[0],"cd")==0){
          free(prompt);
          char* new_cd= cd_sfish(arg_c,arg_v);
          prompt=modif_prompt(new_cd);
       }
       else if(strcmp(arg_v[0],"pwd")==0){
        pwd_sfish(arg_c,arg_v);
       }
       // printf("%s\n",prompt);
       execute_process(arg_c,arg_v);

        // printf("%s\n",cmd);
        /* All your debug print statements should use the macros found in debu.h */
        /* Use the `make debug` target in the makefile to run with these enabled. */
        //info("Length of command entered: %ld\n", strlen(cmd));
        /* You WILL lose points if your shell prints out garbage values. */
        for(int i=0;i<arg_c;i++){
            free(arg_v[i]);
        }
        free(arg_v);
    }

    /* Don't forget to free allocated memory, and close file descriptors. */
    free(cmd);
    free(prompt);

    return EXIT_SUCCESS;
}
