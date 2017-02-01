#include "hw1.h"

// For your helper functions (you may add additional files also)
// DO NOT define a main function here!

char validargs(int argc, char** argv, FILE** in, FILE** out) {
	char ret = 0;

	argv ++;
	argc--;
	if(argc<=0){
		return ret;
	}
	char* first = *argv;
	if(size(first)!=2){
		return ret;
	}
    if(*first!='-'){
    	 return ret;
    }
     first++;
     if ( *first=='s'){

     if(argc>5||argc<=3)
     	return ret;

        argv ++;
   	 	char* second  = *argv;
   	if(checkSecond(second)==0)
   			return ret;

   	argv++;
   	char* INPUT_FILE = *argv;
   	if(*INPUT_FILE=='-'){		//this is for input file
   		*in = stdin;

   	}
   	else
   	{
   	*in = fopen(INPUT_FILE, "r");

   	if (*in==NULL){
   		return ret;
}


   }

    argv++;
    	char* OUTPUT_FILE = *argv;

   	if(*OUTPUT_FILE=='-'){		//this is for output file

   		*out = stdout;
   	}
   	else
   	{
   	*out = fopen(OUTPUT_FILE, "w");
     }
   	char c = 0x40;
   	ret = c|ret;

   	char secCommand = *(++second);
   	if(secCommand == 'd'){
   		c = 0x20;
   		ret = c|ret;
   	}


   	if(argc == 5){
   		argv++;
   		char* number = *argv;
   		 int n =  parseInt(number);
   		 if(n==-1)
   		 	return 0;
   		 int Lenght_of_alpha = size(Alphabet);
   		 int last5 = n%Lenght_of_alpha;
   		 ret =  ret|last5;
	}
	else {

   		 int n =  320;
   		 int Lenght_of_alpha = size(Alphabet);
   		 int last5 = n%Lenght_of_alpha;
   		 ret =  ret|last5;
	}

	return ret;

    }
    else if( *first=='t'){
    	if(argc>4||argc<=3)
     	return ret;

        argv ++;
   	 	char* second  = *argv;
   	    if(checkSecond(second)==0)
   			return ret;
   			argv++;
   	char* INPUT_FILE = *argv;
   	if(*INPUT_FILE=='-'){		//this is for input file
   		*in = stdin;
   	}
   	else
   	{
   	*in = fopen(INPUT_FILE, "r");

   	if (*in==NULL){
   		return ret;
   	}
   }
    argv++;
    	char* OUTPUT_FILE = *argv;

   	if(*OUTPUT_FILE =='-'){		//this is for output file

   		*out = stdout;

   	}
   	else
   	{
   	*out = fopen(OUTPUT_FILE, "w");
     }

   	char secCommand = *(++second);
   	if(secCommand == 'd'){
   		char c = 0x20;
   		ret = c|ret;
   	}
 	     int n =  320;
   		 int Lenght_of_alpha = size(Alphabet);
   		 int last5 = n%Lenght_of_alpha;
   		 ret =  ret|last5;
   		 return ret;
    }
    else if( *first=='h'){
    	char c = 0x80;
    	ret = c | ret;
    	return ret;
    }
    else {
    	return ret;
    }


}
int size(char* s){
	int size=0;
	while (*s!='\0'){
		size++;
		s++;
	}
     return size;
}
int checkSecond(char* s){
	if(size(s)!=2){
		return 0;
	}
    if(*s!='-'){
    	 return 0;
    }
    s++;
	if (*s=='e')
		return 1;
	if (*s == 'd')
		return 1;
	return 0;
}
int parseInt(char* s){
	int length = size(s);
	int loop = length-1;
	int sum =0;
	while (loop!=-1){
		char c = *s;
		if(!(c>='0'&&c<='9')){
			return -1;
		}
		sum =  sum*10 + (c-'0');
	    s++;
	    loop--;
	}
return sum;

}
int alpha_size(char* s){
	int size=0;
	while (*s!='\0'){
		char c = *s;
		if(c>='a'||c<='z'||c>='A'||c<='Z'){
			size++;
		}

		s++;
	}
     return size;
}
int Substitution(int n,FILE** in,FILE** out){

char c;
while ((c = fgetc(*in))!=EOF){
		if(c>='a'&&c<='z'){
			c=c-32;
		}
		fputc(FindC(Alphabet,c,n),*out);
		fflush(*out);
}


return 0;
}

char FindC(char* orig, char c,int n){
	char give = c;
	int length = size(orig);
	for (int i=0; i<length;i++){
		if(*(orig+i)==c){
			int shifted = (n+i)%length;
			if(shifted<0){
				shifted = length+shifted;
			}
			 give = *(orig+shifted);
			 return give;
		}
	}
	return give;
}
char* shfiting(char*s,int n){
	int length = size(Alphabet);
	if(n>0){
		while(n!=0){
			char c = *Alphabet;
			for(int i=0;i<length-1;i++){
				*(Alphabet+i)=*(Alphabet+i+1);
			}
			*(Alphabet+length-1)=c;
			n--;
		}

		return Alphabet;
	}
	else{
		n=n*-1;
		while(n!=0){
			char c = *(Alphabet+length-1);
			for(int i=length-2;i>=0;i--){
				*(Alphabet+i+1)=*(Alphabet+i);
			}
			*(Alphabet)=c;
			n--;
		}
		return Alphabet;
	}

}
int Tutnese_encode(FILE** in, FILE** out){
	char first = fgetc(*in);
	char second = fgetc(*in);
	char* tut;
	int on =1;
	while (on!=0){

		if(equalIgCase(first,second)==1){
			if(first>='a'&& first<='z'){
				fprintf(*out,"%s", "squa");
					if(first=='a'||first=='e'||first=='i'||first=='o'||first=='u'){
						fprintf(*out,"%s", "t");
					}
					tut = findTUT(second);
					if(tut!=NULL){

						printTut(tut,second,&(*out));
					}
					else{
						fputc(second,*out);
					}

				 first = fgetc(*in);
				 second = fgetc(*in);
				 if(((first==EOF)&&(second==EOF)))
				 	on=0;
			    }
			else if(first>='A'&& first<='Z'){
				fprintf(*out,"%s", "Squa");
				if(first=='A'||first=='E'||first=='I'||first=='O'||first=='U'){
					fprintf(*out,"%s", "t");
				}
				tut = findTUT(second);
					if(tut!=NULL){
						printTut(tut,second,&(*out));
					}
					else{
						fputc(second,*out);
					}
				 first = fgetc(*in);
				 second = fgetc(*in);
				 if((first==EOF)&&(second==EOF))
				 	on=0;

			}
				else{
				fputc(first,*out);
				fputc(second,*out);
				 first = fgetc(*in);
				 second = fgetc(*in);
				 if((first==EOF)&&(second==EOF))
				 	on=0;
			}
		}

		else{

			tut = findTUT(first);
					if(tut!=NULL){
						printTut(tut,first,&(*out));
						}
					else{
						fputc(first,*out);
						}
			first = second;
			second = fgetc(*in);
			if((first==EOF)&&(second==EOF))
				 	on=0;
		}
}
return 0;

}
int equalIgCase(char fi,char se){
	if(fi>='a'&& fi<='z'){
		fi = fi-32;
	}
	if(se>='a'&& se<='z')
		se = se-32;
	if(se==fi)
		return 1;
	else
		return 0;
}
char* findTUT(char c){
char* re = NULL;
int length = sizeTut(Tutnese);
if((c>='a'&&c<='z')||(c>='A'&&c<='Z')){
for (int i=0;i<length;i++){
	char check = **(Tutnese+i);
	if(equalIgCase(check,c)==1){
		re = *(Tutnese+i);
		return re;
	}
}
}
return re;
}
int printTut(char* s, char c,FILE** out){
	int length = size(s);
	fputc(c,*out);
	for (int i=1;i<length;i++){
		char pr = *(s+i);
		fputc (pr,*out);
	}

		return 0;

}
int sizeTut(char** s){
	int size=0;
	while (*s!='\0'){
		size++;
		s++;
	}
     return size;
}
int Tutnese_decode(FILE** in, FILE** out){
char c = fgetc(*in);
while (c!=EOF){
	if(isLetter(c)==1){
		if(c=='A'||c=='E'||c=='I'||c=='O'||c=='U'||c=='a'||c=='e'||c=='i'||c=='o'||c=='u'){
			fputc(c,*out);
			 c = fgetc(*in);
		}
		else{
		int check=0;
		cleanBuff(buffer);
		int chance =1;
		while (check<=9&&c!=EOF){
			*(buffer+check)=c;
		char* decode = searchTut(buffer);
		char* doubleT=	checkDT(buffer);
		if(decode!=NULL){
			fputc(*(buffer),*out);
			 c = fgetc(*in);
			 break;
		}
		else if(checkDD(buffer)==1&&chance==1){
				 c = fgetc(*in);
				if(c=='A'||c=='E'||c=='I'||c=='O'||c=='U'||c=='a'||c=='e'||c=='i'||c=='o'||c=='u'){
			 	 char firstC = toLowercase(c);
			 		 if(*(buffer)>='A'&&*(buffer)<='Z')
			 	 		 fputc(firstC-32,*out);
			 		 else
			 	  	  fputc(firstC,*out);

			 		 fputc(c,*out);
			 		 c = fgetc(*in);
			 		 break;
			}
			else{
				check++;
				*(buffer+check)=c;
			      	chance =0;
			      	check--;
			}

		}
		else if(doubleT!=NULL){
			char put = *(buffer+4);
			char firstC = toLowercase(put);
			 	 if(*(buffer)>='A'&&*(buffer)<='Z')
			 	 	 fputc(firstC-32,*out);
			 	 else
			 	    fputc(firstC,*out);

			 	 fputc(put,*out);
			 	 c = fgetc(*in);
				 break;
		}

		check++;
		c = fgetc(*in);
		if(check<=9&&c==EOF)
			return 0;
		}
		if(check==10)
			return 0;
		}
	}
	else{
		fputc(c,*out);
		 c = fgetc(*in);
	}
}
return 1;
}
int isLetter(char c){
	if(c>='a'&&c<='z')
		return 1;
	else if(c>='A'&&c<='Z')
		return 1;
	return 0;
}
char* searchTut(char*s){
	char* tut = NULL;
	int tutSize = sizeTut(Tutnese);
	for (int i=0;i<tutSize;i++){
		int stringL = size(*(Tutnese+i));
		for(int j=0;j<stringL;j++){
			char c = *(s+j);
			if(c>='A'&&c<='Z')
				c=c+32;
			if(c!=(*(*(Tutnese+i)+j)))
				break;
			else if (*(s+j+1)== '\0'&&(*(*(Tutnese+i)+j+1))=='\0')
				return *(Tutnese+i);
		}
	}
	return tut;
}
int cleanBuff(char* c){
	for(int i=0;i<10;i++){
		*(c+i)='\0';
	}
	return 0;
}
int checkDD(char* s){
	if(*s=='s'||*s=='S'){
		if(*(s+1)=='q'&&*(s+2)=='u'&&*(s+3)=='a'&&*(s+4)=='t')
			return 1;
	}
	return 0;
}
char toLowercase(char c){
	if(c>='A'&&c<='Z')
		return c+32;
	return c;
}
char* checkDT(char* s){
if(*s=='s'||*s=='S'){
	if(*(s+1)=='q'&&*(s+2)=='u'&&*(s+3)=='a'){
		char* tu = searchTut((s+4));
			if(tu!=NULL){
				return tu;
			}
	}
}
return NULL;
}