#ifndef SFISH_H
#define SFISH_H
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>


char** parse_command(char* s,int size);
int getArgc(char* s);
int execute_process(int arg_c, char** arg_v);
void print_help(int arg_c,char** arg_v);
char* cd_sfish(int arg_c,char** arg_v);
char* modif_prompt(char* new_cd);
char* pwd_sfish(int arg_c,char** arg_v);
#endif
