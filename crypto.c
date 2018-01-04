#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "crypto.h"

#define RECYCLE 1
#define NO_RECYCLE 0

unsigned char getShift(ArgInfo *argInfoP, unsigned char *shiftp) {
   int c;

   if ((c = getc(argInfoP -> padFile)) == EOF) {
      rewind(argInfoP -> padFile);
      c = getc(argInfoP -> padFile);
      *shiftp = (unsigned char)c;

      return RECYCLE;
   } 

   *shiftp = (unsigned char)c;

   return NO_RECYCLE;
}

void writeByte(unsigned char inByte, FILE *out) {
   /* Use buffered output to speed up writing */
   if (fprintf(out, "%c", inByte) == 0) {
      perror(NULL);
      exit(EXIT_FAILURE);
   }
}

void printPadSizeWarning() {
   fprintf(stderr, "Warning: The pad being used is smaller than the"); 
   fprintf(stderr, " data being encrypted\n");
   fprintf(stderr, "         To ensure perfect secrecy, use a larger");
   fprintf(stderr, " pad\n");
}

void encrypt(ArgInfo *argInfoP) {
   int c;
   unsigned char inByte, shiftBy, warningShown = 0;

   /* Read in another char from inFile */
   while ((c = getc(argInfoP -> inFile)) != EOF) {
      inByte = c;
      if (getShift(argInfoP, &shiftBy) == RECYCLE && !warningShown) {
         /* If end of pad reached before encryption completes, print warning */
         printPadSizeWarning();
         warningShown = 1;
      }

      inByte += shiftBy;
      writeByte(inByte, argInfoP -> outFile);
   }
}

void decrypt(ArgInfo *argInfoP) {
   int c;
   unsigned char inByte, shiftBy;

   /* Read in another char from inFile */
   while ((c = getc(argInfoP -> inFile)) != EOF) {
      inByte = c;
      getShift(argInfoP, &shiftBy);
      inByte -= shiftBy;
      writeByte(inByte, argInfoP -> outFile);
   }
}

void seedRand() {
   time_t seed;

   if ((seed = time(NULL)) == -1) {
      fprintf(stderr, "Unable to retrieve system time\n");
      exit(EXIT_FAILURE);
   }
   srand(seed);
}

void generatePad(ArgInfo *argInfoP) {
   int byteCount;

   seedRand();

   for (byteCount = 0; byteCount < argInfoP -> padSize; byteCount++) {
      writeByte(rand(), argInfoP -> padFile);
   }
}

void evalArgs(ArgInfo *argInfoP) {
   /* Note: It's not possible for op to be anything other than e, d, or g */
   switch (argInfoP -> op) {
      case 'e':
         encrypt(argInfoP);
         break;
      case 'd':
         decrypt(argInfoP);
         break;
      case 'g':
         generatePad(argInfoP);
         break;
      default:
         fprintf(stderr, "op not recognized in file %s at line %d\n", __FILE__,
            __LINE__);
         exit(EXIT_FAILURE);
   }
}
