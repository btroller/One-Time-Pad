#include "crypto.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/random.h>

#define RECYCLE 1
#define NO_RECYCLE 0
#define MAX_GETENTROPY_LEN 256 /* The maximum number of bytes a call to `getentropy(...)` can produce */

/* Puts the next byte from `argInfoP->padFile` to XOR with into `randBP`.
 * Returns `RECYCLE` if the pad had to be rewound, or `NO_RECYCLE` otherwise */
unsigned char getShift(ArgInfo *argInfoP, unsigned char *randBP) {
   int c;

   /* Get the next char in padfile */
   if ((c = getc(argInfoP->padFile)) == EOF) {
      /* If end of padfile is reached, rewind and start from its beginning */
      rewind(argInfoP->padFile);
      c = getc(argInfoP->padFile);
      *randBP = (unsigned char)c;

      return RECYCLE;
   }

   *randBP = (unsigned char)c;

   return NO_RECYCLE;
}

/* Writes the byte `inByte` to the file `out */
void writeByte(unsigned char inByte, FILE *out) {
   /* Attempts to use buffered output to speed up writing */
   if (fprintf(out, "%c", inByte) == 0) {
      perror(NULL);
      exit(EXIT_FAILURE);
   }
}

/* Prints a warning about encrypting with a pad shorter than a message */
void printPadSizeWarning() {
   fprintf(stderr, "Warning: The pad being used is smaller than the");
   fprintf(stderr, " data being encrypted\n");
   fprintf(stderr, "         To ensure perfect secrecy, use a larger");
   fprintf(stderr, " pad\n");
}

/* Encrypts bytes from `argInfoP->inFile` into `argInfoP->outFile` using the pad `argInfoP->padFile` */
void otp_encrypt(ArgInfo *argInfoP) {
   int c;
   unsigned char inByte, randByte, warningShown = 0;

   /* Read in another char from inFile */
   while ((c = getc(argInfoP->inFile)) != EOF) {
      inByte = c;
      /* Get next byte from padfile */
      if (getShift(argInfoP, &randByte) == RECYCLE && !warningShown) {
         /* If end of pad reached before encryption completes, print warning */
         printPadSizeWarning();
         warningShown = 1;
      }

      inByte = inByte ^ randByte;
      writeByte(inByte, argInfoP->outFile);
   }
}

/* Decrypts bytes from `argInfoP->inFile` into `argInfoP->outFile` using the pad `argInfoP->padFile` */
void otp_decrypt(ArgInfo *argInfoP) {
   int c;
   unsigned char inByte, randByte;

   /* Read in another char from inFile */
   while ((c = getc(argInfoP->inFile)) != EOF) {
      inByte = c;
      getShift(argInfoP, &randByte);
      inByte = inByte ^ randByte;
      writeByte(inByte, argInfoP->outFile);
   }
}

/* Generates a pad described in `argInfoP` */
void generatePad(ArgInfo *argInfoP) {
   int numBytesWritten, numBytesToGet;
   unsigned char randBits[MAX_GETENTROPY_LEN];

   for (numBytesWritten = 0; numBytesWritten < argInfoP->padSize;
        numBytesWritten += MAX_GETENTROPY_LEN) {
      if ((numBytesToGet = argInfoP->padSize - numBytesWritten) >
          MAX_GETENTROPY_LEN) {
         /* At most, ask for `MAX_GETENTROPY_LEN` bytes at a time */
         numBytesToGet = MAX_GETENTROPY_LEN;
      }

      /* Get appropriate number of random bytes */
      if (getentropy(randBits, numBytesToGet) == -1) {
         perror(NULL);
         exit(EXIT_FAILURE);
      }

      /* Write results of `getentropy(...)` call to `argInfoP->padFile` */
      if (fwrite(randBits, 1, numBytesToGet, argInfoP->padFile) <
          numBytesToGet) {
         perror(NULL);
         exit(EXIT_FAILURE);
      }
   }
}

/* Evaluates given `argInfoP` */
void evalArgs(ArgInfo *argInfoP) {
   /* Note: It's not possible for op to be anything other than e, d, or g */
   switch (argInfoP->op) {
   case 'e':
      otp_encrypt(argInfoP);
      break;
   case 'd':
      otp_decrypt(argInfoP);
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
