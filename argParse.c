#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "argParse.h"

#define DEFAULT_PADSIZE 1024
#define NOT_FOUND -1

void pUseExit() {
   fprintf(stderr, "Usage: otp -e [-i infile] [-o outfile] -p padfile OR\n");
   fprintf(stderr, "       otp -d [-i infile] [-o outfile] -p padfile OR\n");
   fprintf(stderr, "       otp -g -p padname [padsize]\n");
   exit(EXIT_FAILURE);
}

FILE *openFile(char *fileName, char *mode) {
   FILE *file;

   if ((file = fopen(fileName, mode)) == NULL) {
      perror(NULL);
      exit(EXIT_FAILURE);
   }

   return file;
}

int posInArgv(int argc, char *argv[], char *toFind) {
   int argPos;

   for (argPos = 0; argPos < argc; argPos++) {
      if (strcmp(argv[argPos], toFind) == 0) {
         return argPos;
      }
   }

   return NOT_FOUND;
}

FILE *openArgFile(int argc, char *argv[], int pos, char *mode) {
   FILE *fp;

   if (argc - 1 < pos + 1) {
      /* If there is no argument after the given flag position */
      pUseExit();
   }

   fp = openFile(argv[pos + 1], mode);

   return fp;
}

void checkPadFileEmpty(ArgInfo *argInfoP) {
   if (getc(argInfoP -> padFile) == EOF) {
      fprintf(stderr, "Unable to read from padfile\n");
      fprintf(stderr, "Make sure you have permission to read padfile ");
      fprintf(stderr, "and that it's not empty\n");
      exit(EXIT_FAILURE);
   }
   rewind(argInfoP -> padFile);
}

void parseEncDec(int argc, char *argv[], ArgInfo *argInfoP) {

   int ipos, opos, ppos, remainingArgs = argc - 2;

   if ((ipos = posInArgv(argc, argv, "-i")) == NOT_FOUND) {
      argInfoP -> inFile = stdin;
   }
   else {
      argInfoP -> inFile = openArgFile(argc, argv, ipos, "r");
      remainingArgs -= 2;
   }

   if ((opos = posInArgv(argc, argv, "-o")) == NOT_FOUND) {
      argInfoP -> outFile = stdout;
   }
   else {
      argInfoP -> outFile = openArgFile(argc, argv, opos, "w");
      remainingArgs -= 2;
   }

   if ((ppos = posInArgv(argc, argv, "-p")) == NOT_FOUND) {
      pUseExit();
   }
   else {
      argInfoP -> padFile = openArgFile(argc, argv, ppos, "r");
      remainingArgs -= 2;
   }

   if (remainingArgs > 0) {
      pUseExit();
   }

   checkPadFileEmpty(argInfoP);
}

void parsePad(int argc, char *argv[], ArgInfo *argInfoP) {
   int ppos, remainingArgs = argc - 2;

   if ((ppos = posInArgv(argc, argv, "-p")) == NOT_FOUND) {
      pUseExit();
   }

   argInfoP -> padFile = openArgFile(argc, argv, ppos, "w");
   remainingArgs -= 2;

   if (argc - 1 < ppos + 2) {
      /* If there are no other args after padname in argv, use default size */
      argInfoP -> padSize = DEFAULT_PADSIZE;
   }
   else if (strcmp(argv[ppos + 2], "-g") == 0) {
      /* If the arg following padname is the "-g" flag, use default size */
      argInfoP -> padSize = DEFAULT_PADSIZE;
   }
   /* Try scanning the arg following padname as an int */
   else if (sscanf(argv[ppos + 2], "%d", &(argInfoP -> padSize)) == 0) {
      /* If the arg following padname can't be scanned as an int */
      pUseExit();
   }
   else if (argInfoP -> padSize <= 0) {
      /* If padsize is invalid */
      fprintf(stderr, "padsize must be greater than 0 and at most %d\n", 
         INT_MAX);
      pUseExit();
   }
   remainingArgs -= 1;

   if (remainingArgs > 0) {
      /* If extra args are given in argv */
      pUseExit();
   }
}

void parseArgs(int argc, char *argv[], ArgInfo *argInfoP) {

   /* Check that there is a flag to parse */
   if (argc < 2) {
      pUseExit();
   }

   /* Scan flag's operation-defining character into op */
   argInfoP -> op = '\0';
   if (posInArgv(argc, argv, "-e") > NOT_FOUND) {
      argInfoP -> op = 'e';
   }
   else if (posInArgv(argc, argv, "-d") > NOT_FOUND) {
      argInfoP -> op = 'd';
   }
   else if (posInArgv(argc, argv, "-g") > NOT_FOUND) {
      argInfoP -> op = 'g';
   }

   /* Parse argv based on op */
   switch (argInfoP -> op) {
      case 'e':
         parseEncDec(argc, argv, argInfoP);
         break;
      case 'd':
         parseEncDec(argc, argv, argInfoP);
         break;
      case 'g':
         parsePad(argc, argv, argInfoP);
         break;
      default:
         /* If valid operation flag is not found in argv */
         pUseExit();
         break;
   }
}

void closeFile(FILE *fp) {
   if (fclose(fp) == EOF) {
      perror(NULL);
      exit(EXIT_FAILURE);
   }
}

void closeAllFiles(ArgInfo *argInfoP) {
   closeFile(argInfoP -> inFile);
   closeFile(argInfoP -> outFile);
   closeFile(argInfoP -> padFile);
}

void closeFiles(ArgInfo *argInfoP) {
   switch (argInfoP -> op) {
      case 'e':
         closeAllFiles(argInfoP);
         break;
      case 'd':
         closeAllFiles(argInfoP);
         break;
      case 'g':
         closeFile(argInfoP -> padFile);
         break;
   }
}
