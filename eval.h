#include <stdio.h>
#include "dtypes.h"
#include "glist.h"
#include "debug.h"
#include "print.h"
#include "env.h"

#ifndef eval_h
#define eval_h
thing eval(thing *t, env *env);
void openFileAndEval(char *name);
int scanForVars1(thing *t);
void scanForVars0(glist *list);
#endif
