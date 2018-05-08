#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "env.h"
#include "parser.h"

int main(int argc, char **argv){
  initenv();
  if (argc == 2){
    parseAndEval(argv[1]);
    return 0;
  }

  error("No file supplied", NULL);
  return 1;
}
