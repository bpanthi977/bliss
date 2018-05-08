#include "dtypes.h"
#include "glist.h"
#include "print.h"

void print(thing *t, FILE *f){
  if (t->type == TLIST){ 
    printList(t,f);
  } else{
    printNonList(t,f);
  }
}

void printList(thing *t, FILE *f){
  putc('(', f);
  glist *cell = (glist *)t->data;
  while(1) {
    
    //TODO : Something is fishy here (run b2/first.b)
    // cell->first
    if (cell == NULL || cell->first == NULL){
      break;
    }

    print((thing *)cell->first, f);

    if (cell->rest == NULL)
      break;
    cell = cell->rest;
    putc(' ',f);
  }
  putc(')', f);

}

void printClosure(glist *list, FILE *f){
  while(list != NULL && list->first != NULL){
    fprintf(f, "\n");
    print(&((twothings *)list->first)->first, f);
    fprintf(f, " = ");
    print(&((twothings *)list->first)->second, f);


    list =  list->rest;
  }
}

void printNonList (thing *t, FILE *f){
  switch (t->type){
  case TINT:
    fprintf(f, "%d", (*(int *)t->data));
    break;
  case TSYM:
    fprintf(f, "%s", t->data);
    break;
  case TSTR:
    fprintf(f, "\"%s\"", t->data);
    break;
  case TMACRO:
  case TFUN:
    fprintf(f, "(-%p-):%s",t->data,(t->type == TFUN)?"FUN":"MACRO");
    fprintf(f, "\n\tArgs: ");
    fflush(f);
    thing temp = {TLIST, &((func *)t->data)->args};
    print(&temp, f);
    fflush(f);
    fprintf(f, "\n\tClosedVars:");
    printClosure(&((func *)t->data)->closure, f);
    fprintf(f, "\n\tBody:");
    print(&((func *)t->data)->body, f);
    break;
  default:
    fprintf(f , "Unknown type (%d) ", t->type);
  }
}
