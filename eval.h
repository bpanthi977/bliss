#include <stdio.h>
#include "dtypes.h"
#include "glist.h"
#include "debug.h"
#include "print.h"

#ifndef eval_h
#define eval_h
thing eval(thing *t, env *env);
void openFileAndEval(char *name);
#endif
