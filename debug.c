#include "debug.h"

void error(char message[]){
  printf("ERROR: %s\n", message);
  printDebug();
  fflush(stdout);
}

thing debugThings[200];
int debugThingsCount = 0;

void addDebug(thing *t){
  debugThings[debugThingsCount++] = *t;
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
