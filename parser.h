#include "eval.h"

int isInteger(char *s);
char *getNonListString (FILE *f);
thing parseNonList(FILE *f);
thing parsething(FILE *f);
void parseAndEval(char *name);
