#include "hw1.h"
#include "info.h"
int main(int argc, char **argv) {

    FILE* in;
    FILE* out;

    /* Note: create a variable to assign the result of validargs */
 char result = validargs(argc, argv, &in, &out);

   if (result==0){
    	USAGE(EXIT_FAILURE);
    	return EXIT_FAILURE;
    }


   char c = 0x80;
   char command = c & result;
   if(command == -128){
   USAGE(EXIT_SUCCESS);
   }
   c = 0x40;
   command = c&result;
   if(command == 64){
   	c=0x20;
   	int n = result & 0x1F;
   	char* inp = *(argv+3);
    char* outp = *(argv+4);
    if (*inp=='-')
    	inp = "STDIN";
    if (*outp=='-')
    	outp = "STDOUT";
    	command = c&result;
    	if(command == 32){//decode
    	    Substitution(n*-1,&in,&out);
    	    info_shift(shfiting(Alphabet,n*-1));
    	 	info_amount(n);
 			info_input(inp);
 			info_out(outp);
			info_op("decryption") ;

    	}
    	else{ //encode
    		Substitution(n,&in,&out);
    		info_shift(shfiting(Alphabet,n));
    	 	info_amount(n);
 			info_input(inp);
 			info_out(outp);
			info_op("encryption") ;
		}
 }
   else{
   		c=0x20;
   		command = c&result;
   		if(command == 32){
   		int status = Tutnese_decode(&in,&out);
   			if(status==0)
   			return EXIT_FAILURE;
   		}
   		else{
   			Tutnese_encode(&in,&out);
   		}
   }


   return EXIT_SUCCESS;
}
