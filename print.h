#include "env.h"
#include <stdio.h>

#ifndef print_h
#define print_h
void print(thing *t, FILE *f);
void printList(thing *t, FILE *f);
void printNonList (thing *t, FILE *f);
#endif
