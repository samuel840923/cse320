#ifndef INFO_H
#define INFO_H
#include <stdlib.h>
#include <stdio.h>

#ifdef INFO
#define info_shift(shifted) fprintf(stderr,"%s%s\n","INFO: shifted alphabet: ",shifted)
#define info_amount(n)  fprintf(stderr,"%s%d\n", "INFO: shift amount: ",n);
#define info_input(input) fprintf(stderr,"%s%s\n", "INFO: input file: ",input);
#define info_out(output) fprintf(stderr,"%s%s\n", "INFO: output file: ",output);
#define info_op(op) fprintf(stderr,"%s%s\n", "INFO: operation: ",op);
#else
 #define info_shift(shifted)
#define info_amount(n)
#define info_input(input)
#define info_out(output)
#define info_op(op)
#endif
#endif