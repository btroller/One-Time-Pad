#ifndef ARGPARSE_H
#define ARGPARSE_H

#include <stdio.h>

/* Holds initialized version of inputs given in `argv` or inferred */
typedef struct {
   char op; /* Type of operation - 'g'(enerate), 'd'(ecrypt), 'e'(ncrypt) */
   FILE *inFile; /* File to read from */
   FILE *outFile; /* File to write to */
   FILE *padFile; /* Pad to use or write to */
   int padSize;
} ArgInfo;

void parseArgs(int argc, char *argv[], ArgInfo *argInfoP);

void closeFiles(ArgInfo *argInfoP);

#endif
