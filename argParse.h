#ifndef ARGPARSE_H
#define ARGPARSE_H

#include <stdio.h>

typedef struct {
   char op;
   FILE *inFile;
   FILE *outFile;
   FILE *padFile;
   int padSize;
} ArgInfo;

void parseArgs(int argc, char *argv[], ArgInfo *argInfoP);

void closeFiles(ArgInfo *argInfoP);

#endif
