#include "argParse.h"
#include "crypto.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
   ArgInfo argInfo;

   parseArgs(argc, argv, &argInfo);

   evalArgs(&argInfo);

   closeFiles(&argInfo);

   return EXIT_SUCCESS;
}
