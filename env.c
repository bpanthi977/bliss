#include "env.h"

void addCFunc(char* name, thing (*func)(glist *, env), glist *list){
  twothings *ff = malloc(sizeof(twothings));
  ff->first.type = TSYM;
  ff->first.data = name;
  ff->second.type = TCFUN;
  ff->second.data = func;
  glistPush(ff, list);
}

twothings *addVar(char* name, thing t, glist *list){
  twothings *ff = malloc(sizeof(twothings));
  ff->first.type = TSYM;
  ff->first.data = name;
  ff->second = t;
  glistPush(ff, list);
  return ff;
}

void removeVar(char *name, glist *llist){
  glist *list = llist;
  while(list != NULL && list->first != NULL){
    if (strcmp(name, ((twothings *)list->first)->first.data) == 0){
      glistRemove(list->first, llist);
      break;
    }      
  }
}

void *addVar_Val(twothings* var_val, glist *list){
  glistPush(var_val, list);
}

twothings *findVar(thing symbol, glist *list){
  // Finds variable in var list 
  if (symbol.type != TSYM){
    error("Internal error! findVar expects first argument to be symbol: ", &symbol);
    exit(1);
  }
  while(1){
    if (list->first == NULL){
      return NULL;
    }
    if(strcmp(((twothings*)list->first)->first.data, symbol.data) == 0){
      return list->first;
    }
    list = list->rest;
    if (list == NULL){
      return NULL;
    }
  }
}

void initenv(){
  rootenv.vars.first = NULL;
  rootenv.vars.rest = NULL;

  addVar("NIL", NIL, &rootenv.vars);
  addVar("T", T, &rootenv.vars);
  addVar("NL", NEWLINE, &rootenv.vars);

  addCFunc("+", add, &rootenv.vars);
  addCFunc("-", sub, &rootenv.vars);
  addCFunc("=", areEql, &rootenv.vars);
  addCFunc(">", isGt, &rootenv.vars);
  addCFunc("eval-each", eval_each, &rootenv.vars);
  addCFunc("list", create_list, &rootenv.vars);
  addCFunc("first", first, &rootenv.vars);
  addCFunc("rest", rest, &rootenv.vars);
  addCFunc("print", printF, &rootenv.vars);
  addCFunc("include", include, &rootenv.vars);
  addCFunc("push", push, &rootenv.vars);
}

