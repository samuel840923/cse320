#include "sfish.h"
char** parse_command(char* s,int arg_c){


if(arg_c==0)
	return NULL;
int length = strlen(s);
char** arg_v = malloc(arg_c*sizeof(char*));
int counter=0;
int row=0;
int col=0;
for (int i=0;i<=length;i++){
	if((s[i]!=' '&&s[i]!='\0')){
		int start=i;
		while((s[i]!=' '&&s[i]!='\0')){
			counter++;
			i++;
		}
		arg_v[row] = malloc(counter+1);
		for(int j = start;j<i;j++){
			arg_v[row][col++] = s[j];
		}
		arg_v[row][col] = '\0';
		counter=0;

		row++;
		col=0;
	}
}
return arg_v;
}
int getArgc(char* s){
	int arg_c = 0;
size_t length= strlen(s);
int first =0;
for(int i=0 ;i<=length;i++){
	if(s[i]==' '||s[i]=='\0'){
		if(s[i]=='\0') break;

		while(s[i]==' ')
			i++;
		i--;
		first=0;
	}
	else
		if(first==0){
			arg_c++;
			first=1;
		}
}
return arg_c;
}
int execute_process(int arg_c, char** arg_v){

return 0;
}
void print_help(int arg_c, char** arg_v){
	write(1,"These are the shell builtin command",35);
	write(1,"\n",1);
	write(1,"help : help menu",16);
	write(1,"\n",1);
	write(1,"exit: Exits the shell",strlen("exit: Exits the shell"));
	write(1,"\n",1);
	write(1,"cd: Changes the current working directory",strlen("cd: Changes the current working directory"));
	write(1,"\n",1);
	write(1,"pwd: prints the absolute path of the current working directory",strlen("pwd: prints the absolute path of the current working directory"));
	write(1,"\n",1);
}
 char* cd_sfish(int arg_c,char** arg_v){

	if(arg_c>2){
		write(1,"weird argument",strlen("weird argument"));
		write(1,"\n",1);
		return NULL;
	}
	if(arg_c==1) { //set the home path
		char buff[256];
		char *oldpath = getcwd(buff,256);
		char* homepath = getenv("HOME");
		int check = chdir(homepath);
		if(check==-1){
			write(1,"cannot find the path",strlen("cannot find the path"));
			return NULL;
		}
		check = setenv("OLDPATH",oldpath,1);
		if(check==-1){
			write(1,"cannot set the path",strlen("cannot set the path"));
			write(1,"\n",1);
			return NULL;
		}
		return homepath;
	}
	 if(strcmp(arg_v[1],"-")==0) {
		char* oldpath = getenv("OLDPATH");
		if(oldpath==NULL){
		write(1,"no old path",strlen("no old path"));
		write(1,"\n",1);
		return NULL;
		}
		int status = chdir(oldpath);
		if(status==-1){
			write(1,"cannot change the path",strlen("cannot change the path"));
			write(1,"\n",1);
			return NULL;
		}

		return oldpath;
	}
	if(strcmp(arg_v[1],".")==0){
		char buff[256];
		char *current = getcwd(buff,256);
		if( current==NULL){
			write(1,"no path",strlen("no path"));
			write(1,"\n",1);
			return NULL;
		}
		int status = chdir(current);
		if(status==-1){
			write(1,"cannot change the path",strlen("cannot change the path"));
			write(1,"\n",1);
			return NULL;
		}
		 status = setenv("OLDPATH",current,1);
		if(status==-1){
			write(1,"cannot change the path",strlen("cannot change the path"));
			write(1,"\n",1);
			return NULL;
		}
		return current;
	}
	if(strcmp(arg_v[1],"..")==0){
		char buff[256];
		char* current = getcwd(buff,256);
		int status = setenv("OLDPATH",current,1);
		if(status==-1){
			write(1,"cannot change the path",strlen("cannot change the path"));
			write(1,"\n",1);
			return NULL;
		}
		status = chdir("..");
		if(status==-1){
			write(1,"cannot get parent",strlen("cannot get parent"));
			write(1,"\n",1);
			return NULL;
		}
		current = getcwd(buff,256);
		return current;
	}
		char buff[256];
		char* current = getcwd(buff,256);
		int status = setenv("OLDPATH",current,1);
		if(status==-1){
			write(1,"cannot set the path",strlen("cannot set the path"));
			write(1,"\n",1);
			return NULL;
		}
		status = chdir(arg_v[1]);
		if(status==-1){
			write(1,"cannot change directory",strlen("cannot change directory"));
			write(1,"\n",1);
			return NULL;
		}
		current = getcwd(buff,256);
		return current;
}
char* modif_prompt(char* s){
	if(s==NULL){
	char buff[256];
	char* current = getcwd(buff,256);
	char netid[] = "samuchen : ";
	char * prompt = malloc(strlen(netid)+strlen(current)+4);
	strcat(prompt,netid);
	strcat(prompt,current);
	strcat(prompt," $ ");
	return prompt;
	}
	else{
	char netid[] = "samuchen : ";
	char * prompt = malloc(strlen(netid)+strlen(s)+4);
	strcpy(prompt,netid);
	strcat(prompt,s);
	strcat(prompt," $ ");
	return prompt;
	}
}
char* pwd_sfish(int arg_c,char** arg_v){
	int status=0;
	char buff[256];
	char* current = getcwd(buff,256);
	pid_t pid;
	pid = fork();
	if(pid==0){
		write(1,current,strlen(current));
		write(1,"\n",1);
		exit(0);

	}
	else{
		wait(&status);
	}
return current;
}


