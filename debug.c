#include "debug.h"

void error(char message[], thing *obj){
  printf("ERROR %s ", message);
  if (obj != NULL)
    print(obj, stdout);
  printf("\n");
  printDebug();
  fflush(stdout);
}

thing debugThings[200];
int debugThingsCount = 0;

void addDebug(thing *t){
  debugThings[debugThingsCount++] = *t;
}

void listVars(glist *varVals){
  printf("\nVariable Stack:\n");
  twothings *t;
  int count = 7;
  while (varVals!= NULL && varVals->first !=NULL){
    if (count-- == 0){
      break;
    }
    t = varVals->first;
    print(&t->first, stdout);
    printf(" = ");
    print(&t->second, stdout);
    printf("\n");
    
    varVals = varVals->rest;
  }
}

void removeDebug(){
  debugThingsCount--;
}

void printDebug(){
  printf("StackTrace (%d) :\n", debugThingsCount);
  for (int i = debugThingsCount - 1; i>=0 ; i--){
    printf("%d: ", i);
    print(&debugThings[i], stdout);
    printf("\n");
  }
}
