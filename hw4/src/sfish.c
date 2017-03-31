#include "sfish.h"
char** parse_command(char* s,int arg_c){
if(arg_c==0)
	return NULL;
int length = strlen(s);
char** arg_v = malloc((arg_c+1)*sizeof(char*));
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
arg_v[row]=0;
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
		write(2,"weird argument",strlen("weird argument"));
		write(2,"\n",1);
		return NULL;
	}
	if(arg_c==1) { //set the home path
		int size = 256;
		char *oldpath=malloc(size);
		while(getcwd(oldpath,size)==NULL){
		size=size*2;
		oldpath = realloc(oldpath,size);
		}
		char* homepath = getenv("HOME");
		int check = chdir(homepath);
		if(check==-1||setenv("PWD",homepath,1)==-1){
			write(2,"cannot find the path",strlen("cannot find the path"));
			write(2,"\n",1);
			free(oldpath);
			return NULL;
		}
		check = setenv("OLDPATH",oldpath,1);
		if(check==-1){
			write(2,"cannot set the path",strlen("cannot set the path"));
			write(2,"\n",1);
			free(oldpath);
			return NULL;
		}
		free(oldpath);
		return homepath;
	}
	 if(strcmp(arg_v[1],"-")==0) {
	 	int size = 256;
		char *current=malloc(size);
		while(getcwd(current,size)==NULL){
		size=size*2;
		current = realloc(current,size);
		}
		char* oldpath = getenv("OLDPATH");
		if(oldpath==NULL){
		write(2,"no old path",strlen("no old path"));
		write(2,"\n",1);
		free(current);
		return NULL;
		}
		int status = chdir(oldpath);
		if(status==-1||setenv("PWD",oldpath,1)){
			write(2,"cannot change the path",strlen("cannot change the path"));
			write(2,"\n",1);
			free(current);
			return NULL;
		}
		status = setenv("OLDPATH",current,1);
		if(status==-1){
			write(2,"cannot set the path",strlen("cannot set the path"));
			write(2,"\n",1);
			free(current);
			return NULL;
		}
		free (current);
		return oldpath;
	}
	if(strcmp(arg_v[1],".")==0){
		int size = 256;
		char *current=malloc(size);
		while(getcwd(current,size)==NULL){
		size=size*2;
		current = realloc(current,size);
		}
		if( current==NULL){
			write(2,"no path",strlen("no path"));
			write(2,"\n",1);
			return NULL;
		}
		int status = chdir(current);
		if(status==-1||setenv("PWD",current,1)){
			write(2,"cannot change the path",strlen("cannot change the path"));
			write(2,"\n",1);
			free(current);
			return NULL;
		}
		 status = setenv("OLDPATH",current,1);
		if(status==-1){
			write(2,"cannot set the oldpath",strlen("cannot set the oldpath"));
			write(2,"\n",1);
			free(current);
			return NULL;
		}
		return current;
	}
	if(strcmp(arg_v[1],"..")==0){
		int size = 256;
		char *current=malloc(size);
		while(getcwd(current,size)==NULL){
		size=size*2;
		current = realloc(current,size);
		}
		int status = setenv("OLDPATH",current,1);
		if(status==-1){
			write(2,"cannot set the oldpath",strlen("cannot set the oldpath"));
			write(2,"\n",1);
			free(current);
			return NULL;
		}
		status = chdir("..");
		if(status==-1){
			write(2,"cannot get parent",strlen("cannot get parent"));
			write(2,"\n",1);
			free(current);
			return NULL;
		}
		getcwd(current,size);
		status=setenv("PWD",current,1);
		if(status==-1){
			write(2,"cannot get parent",strlen("cannot get parent"));
			write(2,"\n",1);
			free(current);
			return NULL;
		}
		return current;
	}
		int size = 256;
		char *current=malloc(size);
		while(getcwd(current,size)==NULL){
		size=size*2;
		current = realloc(current,size);
		}

		int status = chdir(arg_v[1]);
		if(status==-1){
			write(2,"cannot change directory",strlen("cannot change directory"));
			write(2,"\n",1);
			free(current);
			return NULL;
		}
		 status = setenv("OLDPATH",current,1);
		if(status==-1){
			write(2,"cannot set the path",strlen("cannot set the path"));
			write(2,"\n",1);
			free(current);
			return NULL;
		}
		 getcwd(current,size);
		 status=setenv("PWD",current,1);
		 if(status==-1){
			write(2,"cannot set the path",strlen("cannot set the path"));
			write(2,"\n",1);
			free(current);
			return NULL;
		}
		return current;
}
char* modif_prompt(char* s){
	if(s==NULL){
	int size = 256;
	char *buff=malloc(size);
	while(getcwd(buff,size)==NULL){
		size=size*2;
		buff = realloc(buff,size);
	}
	char netid[] = "samuchen : ";
	char * prompt = calloc(strlen(netid)+strlen(buff)+4,1);
	strcpy(prompt,netid);
	strcat(prompt,buff);
	strcat(prompt," $ \0");
	reprint = prompt;
	free(buff);
	return prompt;
	}
	else{
	char netid[] = "samuchen : ";
	char * prompt = malloc(strlen(netid)+strlen(s)+4);
	strcpy(prompt,netid);
	strcat(prompt,s);
	strcat(prompt," $ \0");
	reprint=prompt;
	return prompt;
	}
}
char* pwd_sfish(int arg_c,char** arg_v){

	int size = 256;
	char *buff=malloc(size);
	while(getcwd(buff,size)==NULL){
		size=size*2;
		buff = realloc(buff,size);
	}
	write(1,buff,strlen(buff));
	write(1,"\n",1);
	free(buff);

return NULL;
}
int execute_process(int arg_c, char** arg_v,char *tobefree,char *free2,char** envp){
struct stat file_stat;
int st;
int chk = redirection_check(arg_c,arg_v);
	if(chk==1||chk==0||chk==2){
		if(checkValid(arg_c,arg_v)==0){
		redirection_exec(arg_c,arg_v,tobefree,free2,envp);
		return 0;
		}
		else{
			write(2,"i dont support this function",strlen("i dont support this function"));
			write(2,"\n",1);
		return -1;
		}
	}
	if(chk==3){
		if(checkValid_pipe(arg_c,arg_v)==0){
		pipe_exec(arg_c,arg_v,tobefree,free2,envp);
		return 0;
		}
		else{
		write(2,"i dont support this function",strlen("i dont support this function"));
		write(2,"\n",1);
		return -1;
	}
	}
		pid_t pid;
if(strcmp(arg_v[0],"pwd")==0){
	int st;
	if((pid=fork())==0){
	if(arg_c==1)
	pwd_sfish(arg_c,arg_v);
	else{
		  write(2,"invalid arg",strlen("invalid arg"));
          write(2,"\n",1);
	}
	for(int i=0;i<arg_c;i++)
		free(arg_v[i]);
	free(arg_v);
	exit(0);
	}
	else{
		wait(&st);
	}
	return 0;
}
if(strchr(arg_v[0],'/')!=NULL){
	int status = stat(arg_v[0],&file_stat);
	if(status!=0){
		write(2,"file not exist",strlen("file not exist"));
		write(2,"\n",1);
		return -1;
	}
	if((pid=fork())==0){
		int i = execve(arg_v[0],arg_v,envp);
		if(i<0){
			write(2,"cannot execute",strlen("cannot execute"));
			write(2,"\n",1);
		}
		exit(0);
	}
	else{
		wait(&st);
		return 0;
	}
}
else{
	pid_t pid;
	char* search = getenv("PATH");
	char* path=  searchPath(search,arg_v[0]);
	if(path==NULL){
		write(2,"cannot find the path",strlen("cannot find the path"));
		write(2,"\n",1);
		return -1;
	}
	if((pid=fork())==0){
		int i = execve(path,arg_v,envp);
		if(i<0){
			write(2,"cannot execute",strlen("cannot execute"));
			write(2,"\n",1);
			free(path);
		}
		exit(0);
	}
	else{
		wait(&st);
		free(path);
		return 0;
	}

}
return 0;
}
char* searchPath(char* s,char*command){
	char * copy = strdup(s);
	char* first_copy=copy;
int start =0;
int end =0;
int length = strlen(s);
struct stat file_stat;
for(int i=0;i<=length;i++){
	if(s[i]==':'||s[i]=='\0'){
		end =i-1;
		int size = end-start+1;
		char* check = malloc(size+2+strlen(command));// one for '/' one for null character
		memset(check,'\0',size+2+strlen(command));
		strncpy(check,copy,size);
		strcat(check,"/");
		strcat(check,command);
		if(stat(check,&file_stat)==0){
			free(first_copy);
			return check;
		}
		start =i+1;
		copy = copy+size+1;
		free(check);
	}
}
free(first_copy);
return NULL;
}
char** redirection_parse(int arg_c,char** arg_v){
	int count =0;
	for(int i=0;i<arg_c;i++){
		if(strcmp(arg_v[i],">")==0)
			break;
		if(strcmp(arg_v[i],"<")==0)
			break;
		if(strcmp(arg_v[i],"2>")==0)
			break;
		if(strcmp(arg_v[i],"1>")==0)
			break;
		if(strcmp(arg_v[i],"&>")==0)
			break;
		if(strcmp(arg_v[i],">>")==0)
			break;
		if(strcmp(arg_v[i],"<<")==0)
			break;
		count++;
		}

	char** real_arg_v = malloc((count+1)*sizeof(char*));
	for(int i =0;i< count;i++){
		real_arg_v[i] = strdup(arg_v[i]);
	}
	real_arg_v[count] = NULL;
	return real_arg_v;
}
int redirection_check(int arg_c,char** arg_v){

for(int i=0;i<arg_c;i++){
	if(strcmp(arg_v[i],">")==0)
		return 1;
	if(strcmp(arg_v[i],"1>")==0)
		return 2;
	if(strcmp(arg_v[i],"2>")==0)
		return 2;
	if(strcmp(arg_v[i],"&>")==0)
		return 2;
	if(strcmp(arg_v[i],">>")==0)
		return 2;
	if(strcmp(arg_v[i],"<<")==0)
		return 2;
	if(strcmp(arg_v[i],"<")==0)
		return 0;
	if(strcmp(arg_v[i],"|")==0)
		return 3;
}
return -1;
}
int getfile_index(int arg_c,char** arg_v, char* ch){
for(int i=0;i<arg_c;i++){
	if(strcmp(arg_v[i],ch)==0){
		return i+1;
	}
}
return -1;
}
int redirection_getfile(int arg_c,char** arg_v){
	for(int i=0;i<arg_c;i++){
	if(strcmp(arg_v[i],">")==0)
		return i;
	if(strcmp(arg_v[i],"<")==0)
		return i;
	if(strcmp(arg_v[i],"1>")==0)
		return i;
	if(strcmp(arg_v[i],"2>")==0)
		return i;
	if(strcmp(arg_v[i],"&>")==0)
		return i;
	if(strcmp(arg_v[i],">>")==0)
		return i;
	if(strcmp(arg_v[i],"<<")==0)
		return i;
	if(strcmp(arg_v[i],"|")==0)
		return i;
}
return -1;
}
int redirection_exec(int arg_c,char** arg_v,char *tobefree,char *free2,char** envp){
	checkValid(arg_c,arg_v);
	struct stat file_stat;
	int free_yes=0;
    int st;
    pid_t pid;
    char* path= NULL;
    char ** real_arg_v = redirection_parse(arg_c,arg_v);
    int index = redirection_getfile(arg_c,arg_v);
    if(strchr(arg_v[0],'/')!=NULL)
    	path = arg_v[0];
    else {
    	path = searchPath(getenv("PATH"),arg_v[0]);
    	free_yes=1;
    }

    if(strcmp(">",arg_v[index])==0||strcmp(arg_v[index],"1>")==0||strcmp(arg_v[index],">>")==0){
    	if(stat(path,&file_stat)!=0){
    		write(2,"file was not found",strlen("file was not found"));
    		write(2,"\n",1);
    		for(int i=0;i<index;i++)
			free(real_arg_v[i]);
			free(real_arg_v);
			if(free_yes==1)
			free(path);
    		return -1;
			}
		if((pid = fork())==0){
		char* filename = arg_v[index+1];
		if(filename==NULL){
			write(2,"file is empty",strlen("file is empty"));
    		write(2,"\n",1);
			freeAll(free2,tobefree,index,real_arg_v,arg_c,arg_v,free_yes,path);
			exit(-1);
		}
		if(strcmp(arg_v[index],">>")==0){
			int ap_fd=0;
			if((ap_fd=open(filename,O_WRONLY|O_CREAT|O_APPEND,0777))==-1){
			write(2,"cant open",strlen("cant open"));
    		write(2,"\n",1);
			freeAll(free2,tobefree,index,real_arg_v,arg_c,arg_v,free_yes,path);
			exit(-1);
			}
			dup2(ap_fd,1);
			close(ap_fd);
		}
		else{
		int fd = open(filename,O_WRONLY|O_CREAT,0777);
		dup2(fd,1);
		close(fd);
		}
		if(strcmp(arg_v[0],"pwd")==0){
			pwd_sfish(arg_c,arg_v);
			freeAll(free2,tobefree,index,real_arg_v,arg_c,arg_v,free_yes,path);
			exit(0);
		}
		else{
		int i = execve(path,real_arg_v,envp);
		if(i==-1){
			write(2,"somthing is wrong",strlen("somthing is wrong"));
    		write(2,"\n",1);
			freeAll(free2,tobefree,index,real_arg_v,arg_c,arg_v,free_yes,path);
			exit(-1);

				}
		   }
		}
		else{
			wait(&st);
			for(int i=0;i<index;i++)
			free(real_arg_v[i]);
			free(real_arg_v);
			if(free_yes==1)
			free(path);

		}
    }
    else if(strcmp("<",arg_v[index])==0){
    	if(stat(path,&file_stat)!=0){
    		write(2,"file was not found",strlen("file was not found"));
    		write(2,"\n",1);
    		for(int i=0;i<index;i++)
			free(real_arg_v[i]);
			free(real_arg_v);
			if(free_yes==1)
			free(path);
    		return -1;
			}
		if((pid = fork())==0){
			char* filename = arg_v[index+1];
			int fd = open(filename,O_RDONLY);
		if(fd==-1){
			write(2,"file does not exit",strlen("file does not exit"));
    		write(2,"\n",1);
			freeAll(free2,tobefree,index,real_arg_v,arg_c,arg_v,free_yes,path);
			exit(-1);
		}
		dup2(fd,0);
		close(fd);
		if((arg_v[index+2]!=NULL))
		if((strcmp(arg_v[index+2],">")==0)){
			char* outputfile = arg_v[index+3];
			if(outputfile==NULL){
			write(2,"file does not exit",strlen("file does not exit"));
    		write(2,"\n",1);
			freeAll(free2,tobefree,index,real_arg_v,arg_c,arg_v,free_yes,path);
			exit(-1);
			}
			int fd2 = open(outputfile,O_WRONLY|O_CREAT,0777);
			dup2(fd2,1);
			close(fd2);
		}
		if(strcmp(arg_v[0],"pwd")==0){
			pwd_sfish(arg_c,arg_v);
			freeAll(free2,tobefree,index,real_arg_v,arg_c,arg_v,free_yes,path);
			exit(0);
		}
		else{
		int i = execve(path,real_arg_v,envp);

		    if(i==-1){
			write(2,"somthing is wrong",strlen("somthing is wrong"));
			write(2,"\n",1);
			freeAll(free2,tobefree,index,real_arg_v,arg_c,arg_v,free_yes,path);
			exit(-1);
			}
		}
		}
		else{
			wait(&st);
			for(int i=0;i<index;i++)
			free(real_arg_v[i]);
			free(real_arg_v);
			if(free_yes==1)
			free(path);
		 return 0;
		}

    }
    else if(strcmp(arg_v[index],"2>")==0||strcmp(arg_v[index],"&>")==0){
    	if(stat(path,&file_stat)!=0){
    		write(2,"file was not found",strlen("file was not found"));
    		write(2,"\n",1);
    		for(int i=0;i<index;i++)
			free(real_arg_v[i]);
			free(real_arg_v);
			if(free_yes==1)
			free(path);
    		return -1;
			}
		if((pid = fork())==0){
		char* filename = arg_v[index+1];
		int fd = open(filename,O_WRONLY|O_CREAT,0777);
		if(arg_v[index][0]=='&'){
			if(fd==-1||dup2(fd,1)==-1){
			freeAll(free2,tobefree,index,real_arg_v,arg_c,arg_v,free_yes,path);
			exit(0);
		}
		}
		if(fd==-1||dup2(fd,2)==-1){
			freeAll(free2,tobefree,index,real_arg_v,arg_c,arg_v,free_yes,path);
			exit(0);
		}
		close(fd);
		if(strcmp(arg_v[0],"pwd")==0){
			pwd_sfish(arg_c,arg_v);
			freeAll(free2,tobefree,index,real_arg_v,arg_c,arg_v,free_yes,path);
			exit(0);
		}
		else{
		int i = execve(path,real_arg_v,envp);
		if(i==-1){
			write(2,"somthing is wrong",strlen("somthing is wrong"));
    		write(2,"\n",1);
			freeAll(free2,tobefree,index,real_arg_v,arg_c,arg_v,free_yes,path);
			exit(-1);
				}
		   }
		}
		else{
			wait(&st);
			for(int i=0;i<index;i++)
			free(real_arg_v[i]);
			free(real_arg_v);
			if(free_yes==1)
			free(path);
		}
    }
    else if(strcmp(arg_v[index],"<<")==0){
    	if(stat(path,&file_stat)!=0){
    		write(2,"file was not found",strlen("file was not found"));
    		write(2,"\n",1);
    		for(int i=0;i<index;i++)
			free(real_arg_v[i]);
			free(real_arg_v);
			if(free_yes==1)
			free(path);
    		return -1;
			}

		if((pid = fork())==0){
		int pd[2];
		pipe(pd);
		char* target = arg_v[index+1];
		char* targ = malloc(strlen(target)+1);
		strcpy(targ,target);
		strcat(targ,"\n");
		int size = 50;
		char *test = malloc(size);
		write(1,">",1);
		while(fgets(test,size,stdin)==NULL){
			size=size*2;
			test = realloc(test,size);
		}
		while(strcmp(targ,test)!=0){
			write(1,">",1);
			if(test!=NULL)
			write(pd[1],test,strlen(test));
			else{
				size=size*2;
				test = realloc(test,size);
			}
			fgets(test,size,stdin);
		}
		free(targ);
		free(test);
		dup2(pd[0],0);
		close(pd[1]);

		if(strcmp(arg_v[0],"pwd")==0){
			pwd_sfish(arg_c,arg_v);
			close(pd[1]);
			close(pd[0]);
			freeAll(free2,tobefree,index,real_arg_v,arg_c,arg_v,free_yes,path);
			exit(0);
		}
		else{
		int i = execve(path,real_arg_v,envp);
		if(i==-1){
			write(2,"somthing is wrong",strlen("somthing is wrong"));
    		write(2,"\n",1);
			freeAll(free2,tobefree,index,real_arg_v,arg_c,arg_v,free_yes,path);
			exit(-1);
				}
		   }
		}
		else{

			wait(&st);
			for(int i=0;i<index;i++)
			free(real_arg_v[i]);
			free(real_arg_v);
			if(free_yes==1)
			free(path);
		}
    }



 return 0;
}
void freeAll(char* free2,char* tobefree,int index,char** real_arg_v,int arg_c,char** arg_v,int free_yes,char* path){
	for(int i=0;i<index;i++)
			free(real_arg_v[i]);
			free(real_arg_v);
			for(int i=0;i<arg_c;i++)
			free(arg_v[i]);
			free(arg_v);
			if(free_yes==1)
			free(path);
			free(tobefree);
			free(free2);
}
int pipe_exec(int arg_c,char** arg_v,char *tobefree,char *free2,char** envp){
	int times = pipe_number(arg_c,arg_v);
	int fd[2];
	int fd2[2];
	pipe(fd);
	char** firstarg = pipe_parse_1(arg_c,arg_v);
	int check = fork_first(firstarg,fd,envp);
	if (check ==-1){
		write(2,"cannot find the program",strlen("cannot find the program"));
		write(2,"\n",1);
		close(fd[0]);
	    close(fd[1]);
	    int length = find_argC(firstarg);
	    for(int i=0 ;i<length;i++)
	    	free(firstarg[i]);
	    free(firstarg);
		return -1;
		}
	if(times==2){
		 pipe(fd2);
	}
	char** secondarg= pipe_parse_2(arg_c,arg_v);
	check = fork_second(secondarg,fd,fd2,times,envp);
	if (check ==-1){
		close(fd[0]);
	    close(fd[1]);
	    int length = find_argC(secondarg);
	    for(int i=0 ;i<length;i++)
	    	free(secondarg[i]);
	    free(secondarg);
	    if(times==2){
	    	close(fd2[0]);
			close(fd2[1]);
	    }
		return -1;
		}
	if(times==2){
		close(fd[0]);
		close(fd[1]);
	    char** thirdarg= pipe_parse_3(arg_c,arg_v);
	    fork_third(thirdarg,fd2,envp);
	    int length = find_argC(thirdarg);
	    for(int i=0 ;i<length;i++)
	    	free(thirdarg[i]);
	       free(thirdarg);
	}
	int pid;
	int st;

	if(times ==2){
	close(fd2[0]);
	close(fd2[1]);
	}
	else{
		close(fd[0]);
	     close(fd[1]);
	}
	while ((pid = wait(&st)) != -1)
		;

	return 0;

}

int pipe_number(int arg_c, char** arg_v){
int count =0;
for (int i=0;i<arg_c;i++){
if(strcmp("|",arg_v[i])==0)
	count++;
}
return count;
}
char** pipe_parse_1(int arg_c,char** arg_v){
	int count =0;
	for(int i=0;i<arg_c;i++){
		if(strcmp(arg_v[i],"|")==0)
			break;
		count++;
		}
	char** real_arg_v = malloc((count+1)*sizeof(char*));
	for(int i =0;i< count;i++){
		real_arg_v[i] = strdup(arg_v[i]);
	}
	real_arg_v[count] = NULL;
	return real_arg_v;
}
char** pipe_parse_2(int arg_c,char** arg_v){
	int count =0;
	int start = redirection_getfile(arg_c,arg_v);
	for(int i=start+1;i<arg_c;i++){
		if(strcmp(arg_v[i],"|")==0)
			break;
		count++;
		}

	char** real_arg_v = malloc((count+1)*sizeof(char*));
	for(int i =0;i< count;i++){
		real_arg_v[i] = strdup(arg_v[start+i+1]);
	}
	real_arg_v[count] = NULL;
	return real_arg_v;
}
char** pipe_parse_3(int arg_c,char** arg_v){
	int count =0;
	int counter=0;;
	int index =0;
	for(int i=0;i<arg_c;i++){
		if(strcmp(arg_v[i],"|")==0)
			counter++;
		if(counter==2){
			index =i;
			index++;
			break;
		}
		}
		count = arg_c-index;
	char** real_arg_v = malloc((count+1)*sizeof(char*));
	for(int i =0;i< count;i++){
		real_arg_v[i] = strdup(arg_v[index+i]);
	}
	real_arg_v[count] = NULL;
	return real_arg_v;
}
int find_argC(char** arg_v){
	int count=0;
	int index =0;
while(arg_v[index]!=NULL){
	count++;
	index++;
}
return count;
}
int fork_first(char** arg_v,int pd[],char** envp){
	struct stat file_stat;
	pid_t pid;
	char* path= NULL;
	int free_yes=0;
	if(arg_v[0]==NULL)
		return -1;
	if(strchr(arg_v[0],'/')!=NULL){
		path = arg_v[0];
	   }
	else{
		path = searchPath(getenv("PATH"),arg_v[0]);
		free_yes=1;
		}
		if(stat(path,&file_stat)==-1){
			write(2,"cannot find file",strlen("cannot find file"));
			write(2,"\n",1);
			if(free_yes==1)
			free(path);
	        return -1;
	    }
		if((pid=fork())==0){
			dup2(pd[1],1);
			close(pd[0]);
			if(strcmp(arg_v[0],"pwd")==0){
				pwd_sfish(find_argC(arg_v),arg_v);
				if(free_yes==1)
					free(path);
			int length = find_argC(arg_v);
	  		for(int i=0 ;i<length;i++)
	    		free(arg_v[i]);
	   		free(arg_v);
			exit(0);
			}
			else if(execve(path,arg_v,envp)==-1){
				write(2,"cannot execute",strlen("cannot execute"));
				write(2,"\n",1);
				if(free_yes==1)
					free(path);
			int length = find_argC(arg_v);
	  		for(int i=0 ;i<length;i++)
	    		free(arg_v[i]);
	   		free(arg_v);
			exit(0);
			}
		}
		else{
			if(free_yes==1)
			free(path);
		}
	return 0;
}
int fork_second(char** arg_v,int pd[],int pd2[],int times,char** envp){
	struct stat file_stat;
	pid_t pid;
	char* path= NULL;
	int free_yes=0;
	if(arg_v[0]==NULL){
		printf("no program entered\n");
		return -1;
	}
	if(strchr(arg_v[0],'/')!=NULL){
		path = arg_v[0];
	   }
	else{
		path = searchPath(getenv("PATH"),arg_v[0]);
		free_yes=1;
		}
		if(stat(path,&file_stat)==-1){
			write(2,"cannot find path",strlen("cannot find path"));
			write(2,"\n",1);
			if(free_yes==1)
			free(path);
	        return -1;
	    }
		if((pid=fork())==0){
			dup2(pd[0],0);
			if(times==2){
			dup2(pd2[1],1);
			close(pd2[0]);
			}
			close(pd[1]);
			if(strcmp(arg_v[0],"pwd")==0){
				pwd_sfish(find_argC(arg_v),arg_v);
				if(free_yes==1)
					free(path);
			int length = find_argC(arg_v);
	  		for(int i=0 ;i<length;i++)
	    		free(arg_v[i]);
	   		free(arg_v);
			exit(0);
			}
			if(execve(path,arg_v,envp)==-1){
				write(2,"cannot execute",strlen("cannot execute"));
				write(2,"\n",1);
				if(free_yes==1)
					free(path);
				 int length = find_argC(arg_v);
	  			  for(int i=0 ;i<length;i++)
	    				free(arg_v[i]);
	   				 free(arg_v);
	   				 exit(0);
			}
		}
		else{
			if(free_yes==1)
			free(path);
		}
	return 0;
}
int fork_third(char** arg_v,int pd[],char** envp){
	struct stat file_stat;
	pid_t pid;
	char* path= NULL;
	int free_yes=0;
	if(arg_v[0]==NULL){
		printf("no program entered\n");
		return -1;
	}
	if(strchr(arg_v[0],'/')!=NULL){
		path = arg_v[0];
	   }
	else{
		path = searchPath(getenv("PATH"),arg_v[0]);
		free_yes=1;
		}
		if(stat(path,&file_stat)==-1){
			write(2,"cannot find the program",strlen("cannot find the program"));
			write(2,"\n",1);
			if(free_yes==1)
			free(path);
	        return -1;
	    }
		if((pid=fork())==0){
			dup2(pd[0],0);
			close(pd[1]);
			if(strcmp(arg_v[0],"pwd")==0){
				pwd_sfish(find_argC(arg_v),arg_v);
				if(free_yes==1)
					free(path);
			int length = find_argC(arg_v);
	  		for(int i=0 ;i<length;i++)
	    		free(arg_v[i]);
	   		free(arg_v);
			exit(0);
			}
			if(execve(path,arg_v,envp)==-1){
				write(2,"cannot execute",strlen("cannot execute"));
				write(2,"\n",1);
				if(free_yes==1)
					free(path);
				 int length = find_argC(arg_v);
	    for(int i=0 ;i<length;i++)
	    	free(arg_v[i]);
	    	free(arg_v);
				exit(0);
			}
		}
		else{
			if(free_yes==1)
			free(path);
		}
	return 0;
}
int checkValid(int arg_c,char** arg_v){
int output_d =0;
int input_d =0;
int append =0;
int one_out=0;
int two_out=0;
int both_out=0;
int here =0;
for(int i=0;i<arg_c;i++){
	if(strcmp(arg_v[i],">")==0)
		output_d++;
	if(strcmp(arg_v[i],"<")==0)
		input_d++;
	if(strcmp(arg_v[i],">>")==0)
		append++;
	if(strcmp(arg_v[i],"1>")==0)
		one_out++;
	if(strcmp(arg_v[i],"2>")==0)
		two_out++;
	if(strcmp(arg_v[i],"&>")==0)
		both_out++;
	if(strcmp(arg_v[i],"<<")==0)
		here++;
	if(strcmp(arg_v[i],"|")==0)
		return -1;

}
int sum = both_out+two_out+one_out+append+here;
if(sum>1||sum+input_d>1||sum+output_d>1)
	return -1;
if(input_d>1||output_d>1)
	return -1;
if(input_d==1&&output_d==1){
	char* test= NULL;
for(int i=0;i<arg_c;i++){
	if(strcmp(arg_v[i],">")==0){
		test = arg_v[i];
		break;
	}
	if(strcmp(arg_v[i],"<")==0){
		test = arg_v[i];
		break;
	}
}
if(strcmp(test,">")==0){
	return -1;
}
}
return 0;
}
int checkValid_pipe(int arg_c,char** arg_v){
	int pipe =0;
	for(int i=0;i<arg_c;i++){
	if(strcmp(arg_v[i],">")==0){
		return -1;
	}
	if(strcmp(arg_v[i],"<")==0){
		return -1;

	}
	if(strcmp(arg_v[i],"1>")==0){
		return -1;

	}
	if(strcmp(arg_v[i],"2>")==0){
		return -1;

	}
	if(strcmp(arg_v[i],"&>")==0){
		return -1;

	}
	if(strcmp(arg_v[i],">>")==0){
		return -1;

	}

	if(strcmp(arg_v[i],"|")==0){
		pipe++;

	}
}
	if(pipe>2)
		return -1;
	return 0;
}
int conversion_string(char*s){
	int num = 0;
	int length = strlen(s);
	for(int i=0;i<length;i++){
		if(s[i]>='0'&&s[i]<='9'){
			num = (num*10)+(s[i]-'0');
		}
		else
			return -1;
	}
	if(num!=0)
	flag = num;
	return num;
}
void sigarm_handler(int length){
char buffer[12];
parse_int(flag,buffer);
write(1,"\n",1);
	write(1,"Your ",5);
	write(1,buffer,strlen(buffer));
	write(1," second timer has finished!",27);
	write(1,"\n",1);
	write(1,reprint,strlen(reprint));
}
void sigChild_handler(int sig, siginfo_t *help, void *no){
	write(1,"Child with PID ",strlen("Child with PID "));
	pid_t pid = help->si_pid;
	char buffer[12];
	parse_int(pid,buffer);
	write(1,buffer,strlen(buffer));
	write(1," has died. It spent ",strlen(" has died. It spent "));
	clock_t total_time = (help->si_utime)+(help->si_stime);
	total_time = total_time * 1000;
	total_time = (total_time/sysconf(_SC_CLK_TCK));
	char buffer_time[12];
	parse_int(total_time,buffer_time);
	write(1,buffer_time,strlen(buffer_time));
	write(1," milliseconds",strlen(" milliseconds"));
	write(1,"\n",1);
}
void SIGUSR2_handler(int f){
	write(1,"\n",1);
	write(1,"Well that was easy.",strlen("Well that was easy."));
	write(1,"\n",1);
	write(1,reprint,strlen(reprint));
}
void parse_int(long int a, char buff[]){
if(a!=0){
int size =0;
int t = a;
while(t!=0){
	size++;
	t =t/10;
}
t =a;
int len = size;
size = size-1;
while(t!=0){
buff[size] =(t% 10)+'0';
t =t/10;
size--;
}
buff[len] ='\0';
}
else{
	buff[0] ='0';
	buff[1] ='\0';
}
}
/*
// p6
void print_job(clock_t currTime){
	printf("PROCESS ID\tPROGRAM WITH ARGS\tHOW LONG ITS BEEN RUNNING\n");
	printf("----------\t-----------------\t---------------------------\n");
	struct pid_background *now = back_head;
	process_list();
	while(now!=NULL){
		pid_t id= now->pid;
		char **  prog = now -> arg;
		clock_t time = (now ->time)-currTime;
		int length = now->argc;

		printf("%d\t\t",id);
		for(int i =0;i<length-1;i++){
			printf("%s ",prog[i]);
		}
		printf("\t");
		printf("%ld\n",time);
		now = now -> next;
	}
}
*/
/*
int make_backgound(clock_t currTime, int argc, char** arg_v,char** envp ){
	int st;
	arg_v[argc-1]=0;
	pid_t pid;
	struct stat file_stat;
	char* path;

if(strchr(arg_v[0],'/')!=NULL){
	path = arg_v[0];
}
else{
char* search = getenv("PATH");
path =  searchPath(search,arg_v[0]);
}
if(path==NULL){
		write(2,"cannot find the path",strlen("cannot find the path"));
		write(2,"\n",1);
		return -1;
}
int status = stat(path,&file_stat);
	if(status!=0){
		write(2,"file not exist",strlen("file not exist"));
		write(2,"\n",1);
		free(path);
		return -1;
	}

	if((pid=fork())==0){
		 setpgid(pid, 0);
		int i = execve(path,arg_v,envp);
		if(i<0){

			write(2,"cannot execute",strlen("cannot execute"));
			write(2,"\n",1);
			free(path);
		}
		exit(0);
	}
	else{
		waitpid(-1,&st,WNOHANG);
		if(WIFCONTINUED(st)){
			return 0;
		}
	}
	return 0;
}

void process_list(){

}
*/

