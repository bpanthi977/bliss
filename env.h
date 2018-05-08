#ifndef env_h
#define env_h

#include <stdio.h>
#include "dtypes.h"
#include "glist.h"
#include "stdlibrary.h"

void addCFunc(char* name, thing (*func)(glist *, env), glist *list);
twothings *addVar(char* name, thing t, glist *list);
void *addVar_Val(twothings* var_val, glist *list);
twothings *findVar(thing symbol, glist *list);
void initenv();
env rootenv;
#endif
