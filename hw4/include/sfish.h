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
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

int flag ;
char* reprint;
char** parse_command(char* s,int size);
int getArgc(char* s);
int execute_process(int arg_c, char** arg_v,char *tobefree,char *free2,char** envp);
void print_help(int arg_c,char** arg_v);
char* cd_sfish(int arg_c,char** arg_v);
char* modif_prompt(char* new_cd);
char* pwd_sfish(int arg_c,char** arg_v);
char* searchPath(char* s,char*command);
char** redirection_parse(int arg_c,char** arg_v);
int redirection_check(int arg_c,char** arg_v);
int getfile_index(int arg_c,char** arg_v, char* ch);
int redirection_exec(int arg_c,char** arg_v,char *tobefree,char *free2,char** envp);
int redirection_getfile(int arg_c,char** arg_v);
void freeAll(char* free2,char* tobefree,int index,char** real_arg_v,int arg_c,char** arg_v,int free_yes,char* path);
int pipe_exec(int arg_c,char** arg_v,char *tobefree,char *free2,char** envp);
int pipe_number(int arg_c, char** arg_v);
char** pipe_parse_1(int arg_c,char** arg_v);
char** pipe_parse_2(int arg_c,char** arg_v);
char** pipe_parse_3(int arg_c,char** arg_v);
int find_argC(char** arg_v);
int fork_first(char** arg_v,int pd[],char** envp);
int fork_second(char** arg_v,int pd[],int pd2[],int times,char** envp);
int fork_third(char** arg_v,int pd[],char** envp);
int checkValid(int arg_c,char** arg_v);
int checkValid_pipe(int arg_c,char** arg_v);
int conversion_string(char*s);
void sigarm_handler(int length);
void sigChild_handler(int sig, siginfo_t *help, void *no);
void SIGUSR2_handler(int f);
void parse_int(long int a, char buff[]);
#endif
