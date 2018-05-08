#ifndef env_h
#define env_h

#include <stdio.h>
#include "dtypes.h"
#include "glist.h"
#include "stdlibrary.h"

void addCFunc(char* name, thing (*func)(glist *, env), glist *list);
void addVar(char* name, thing t, glist *list);
twothings *findVar(thing symbol, glist *list);
void initenv();
env rootenv;
#endif
