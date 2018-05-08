#ifndef debug_h
#define debug_h

#include <stdio.h>
#include "dtypes.h"

void error(char message[]);
void addDebug(thing *t);
void removeDebug();
void printDebug();
#endif
