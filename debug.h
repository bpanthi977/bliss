#ifndef debug_h
#define debug_h

#include <stdio.h>
#include "dtypes.h"

void error(char message[], thing *);
void addDebug(thing *t);
void removeDebug();
void printDebug();
void listVars(glist *varVals);
#endif
