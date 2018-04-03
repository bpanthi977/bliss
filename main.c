#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "dtypes.h"
#include "glist.c"
#include "parser.c"
#include "stdlibrary.c"
/* #include "eval.c" */

void error(char message[]){
  printf("ERROR: %s\n", message);
  fflush(stdout);
}


void *parsethingdata(thing *a);

env rootenv;



void addCFunc(char* name, thing (*func)(glist *, env), glist *list){
  twothings *ff = malloc(sizeof(twothings));
  ff->first.type = TSYM;
  ff->first.data = name;
  ff->second.type = TCFUN;
  ff->second.data = func;
  glistPush(ff, list);
}

void initenv(){
  rootenv.vars.first = NULL;
  rootenv.vars.rest = NULL;

  twothings *v1 = malloc(sizeof(twothings));
  v1->first = NIL;
  v1->second = NIL;

  glistPush(v1, &rootenv.vars);

  addCFunc("+", add, &rootenv.vars);
  /* rootenv.funcs.first = NULL; */
  /* rootenv.funcs.rest = NULL; */

  /* thing arg1sym = {TSYM, "arg1", NULL}; */
  /* parsethingdata(&arg1sym); */
  /* thing arg1val = {TSTR, "\"main\"", NULL}; */
  /* parsethingdata(&arg1val); */
  /* var arg1 = {arg1sym, arg1val}; */
  /* glistPush(&arg1, &rootenv.vars); */

  /* thing funaddsym = {TSYM, "+", NULL}; */
  /* parsethingdata(&funaddsym); */
  /* thing funadd = {TFUN, "", add}; */
  /* glist fargs; // Not needed now */
  
  /* /\* func add = {funaddsym, fargs, 2, funadd}; *\/ */
  /* func *add = malloc(sizeof(func)); */
  /* add->symbol = funaddsym; */
  /* add->args = fargs; */
  /* add->argc = 2; */
  /* add->value = funadd; */
  /* glistPush(add, &rootenv.funcs); */

  
  /* thing funmultisym = {TSYM, "*", NULL}; */
  /* parsethingdata(&funmultisym); */
  /* thing funmulti = {TFUN, "", multi}; */
  /* glist fmargs; // Not needed now */
  
  /* /\* func add = {funaddsym, fargs, 2, funadd}; *\/ */
  /* func *multi = malloc(sizeof(func)); */
  /* multi->symbol = funmultisym; */
  /* multi->args = fmargs; */
  /* multi->argc = 2; */
  /* multi->value = funmulti; */
  /* glistPush(multi, &rootenv.funcs); */

  
  /* thing printisym = {TSYM, "printi", NULL}; */
  /* parsethingdata(&printisym); */
  
  /* thing funprinti = {TFUN, "", printi}; */
  /* glist fprintiargs; // Not needed now */
  /* func printi = {printisym, fprintiargs, 1, funprinti}; */
  /* func *printif = malloc(sizeof(func)); */
  /* *printif = printi; */
  /* glistPush(printif, &rootenv.funcs); */
  /* printfunc(rootenv.funcs.rest->first); */
  /* printfunc(rootenv.funcs.first); */
}

twothings *findVar(thing symbol, glist *list){
  if (symbol.type != TSYM){
    error("Internal error! findVar expects first argument to be symbol");
    exit(1);
  }
  while(1){
    if (list->first == NULL){
      return NULL;
    }
    /* printf("%s",((twothings*)list->first)->first.data ); */
    if(strcmp(((twothings*)list->first)->first.data, symbol.data) == 0){
      return list->first;
    }
    list = list->rest;
    if (list == NULL){
      return NULL;
    }
  }
}

thing eval(thing *t, env *env){
  /* print(t, stdout); */
  /* printf("\n"); */
  /* fflush(stdout); */
  if (t->type == TSYM){
    twothings* tt = findVar(*t, &env->vars);
    if (tt == NULL){
      error("Symbol value unknown");
      return NIL;
    }
    return tt->second;
  }
  else if (t->type == TLIST){ 
    // first thing of list is a function or a macro or a special function
    glist *list = t->data;
    thing *first = list->first;
    if (first->type == TSYM){
      if (strcmp(first->data, "if") == 0){
	// special cases
	thing arg1 = eval(list->rest->first, env);
	if (arg1.data == &NIL){
	  thing arg3 = eval(list->rest->rest->rest->first, env);
	  return arg3;
	}
	thing arg2 = eval(list->rest->rest->first, env);
	return arg2;
      }
      
      if (strcmp(first->data, "func") == 0){
	/* thing arg1 = eval(t->list.rest->first, env); //function name */
	func *function = malloc(sizeof(func));   //function object (internal representation)
	function->args = *((glist *)((thing *)list->rest->first)->data);
	function->body.type = TLIST;
	function->body.data = list->rest->rest;
	thing fthing = {TFUN, function};
	return fthing;
      }

      if (strcmp(first->data, "def") == 0){
	thing arg1;
	if (((thing *)list->rest->first)->type != TSYM)
	  arg1 = eval(list->rest->first, env);
	else
	  arg1 = *(thing *)list->rest->first;

	if (arg1.type != TSYM){
	  error("def name not a symbol");
	  return NIL;
	}

	twothings *entry = (twothings *)malloc(sizeof(twothings));
	entry->first = arg1;
	entry->second = eval(list->rest->rest->first, env); // value

	func *function = (func *)entry->second.data;
	glistPush(entry, &env->vars);
	return entry->second;
      }
    }

    // Default case, first item is a function
    // could have directly evaluated but for efficiency
   
    thing funcThing;
    if (first->type == TSYM){
      twothings *tt = findVar(*first, &env->vars);
      if (tt == NULL){
	error("Unknown symbol");
	return NIL;
      }
      if (tt->second.type != TFUN && tt->second.type != TCFUN){
	error("Symbol not a function");
	return NIL;
      }
      funcThing = tt->second;
      /* f = (func *)tt->second.data; */
    } else if (first->type == TLIST){
      thing aeval = eval(first, env); // after evaluation
      if (aeval.type != TFUN && aeval.type != TCFUN){
	error("First thing should evaluate to a function got:" );
	print(&aeval, stdout);
	return NIL;
      }
      /* f = (func *)aeval.data; */
      funcThing = aeval;
    } else {
      error("First thing not a function");
      return NIL;
    }

    if (funcThing.type == TCFUN){
      //thing (*function)(glist* p,env* q);
      thing (*function)(glist*,struct env*);
      function = funcThing.data;
      thing rVal = function(list->rest, env);
      return rVal;
    }
    func *f = (func *) funcThing.data;
    
    // Push arguments to environment
    glist *rargs = &f->args; // required args
    glist *gargs = list->rest; // given args
    glist argVars = {NULL, NULL}; 
    while(1){
      if (rargs == NULL || rargs->first == NULL)
	break;
      if (gargs == NULL || gargs->first == NULL){
	error("Arguments insufficient");
	return NIL;
      }
      thing *rarg = (thing *)rargs->first;
      thing *garg = (thing *)gargs->first;

      twothings *argVar = malloc(sizeof(twothings));

      argVar->first = *rarg;
      argVar->second = eval(garg, env);
      if (argVar->first.type != TSYM){
	// TODO: this checking should be done when func is evaled
	error("Argument Variable not a symbol");
	return NIL;
      }
      glistPush(argVar, &env->vars);
      glistPush(argVar, &argVars);

      if (rargs->rest == NULL)
	break;
      if (gargs->rest == NULL){
	error("Arguments insufficient");
	return NIL;
      }
      rargs = rargs->rest;
      gargs = gargs->rest;
    }
    
    // Evaluate function
    thing rVal;
    glist *forms = (glist *)f->body.data;
    while (1){
      if (forms == NULL || forms->first == NULL )
	break;
      rVal = eval((thing *)forms->first, env);
      if (forms->rest == NULL)
	break;
      forms = forms->rest;
    }
    
    // Remove arguments
    rargs = &f->args;
    while(1){
      
      if (argVars.first == NULL)
	break;
      
      glistRemove(argVars.first, &env->vars);
      free(argVars.first);
      if (argVars.rest == NULL)
	break;
      argVars = *argVars.rest;      
    }
    return rVal;
  }
  else {
    return *t;
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

void printNonList (thing *t, FILE *f){
  switch (t->type){
  case TINT:
    fprintf(f, "%d:INT", (*(int *)t->data));
    break;
  case TSYM:
    fprintf(f, "%s:SYM", t->data);
    break;
  case TSTR:
    fprintf(f, "%s:STR", t->data);
    break;
  case TFUN:
  fprintf(f, "(-%p-):FUN", t->data);
    fflush(f);
    fprintf(f, "\n\tArgs: ");
    thing temp = {TLIST, &((func *)t->data)->args};
    print(&temp, f);
    printf("\n\t Body:");
    print(&((func *)t->data)->body, f);
    break;
  default:
    fprintf(f , "Unknown type (%d) ", t->type);
  }
    /* printf("%s:%s", a.string, (a.type==TINT?"INT":a.type==TSTR?"STR":"SYM")); */
}

void print(thing *t, FILE *f){
  if (t->type == TLIST){
 
    printList(t,f);
  } else{
    printNonList(t,f);
  }
}

int main(int argc, char **argv){
  FILE *fp;
  thing t;
  initenv();
  if (argc == 2){
    if ((fp = fopen(argv[1], "r")) == NULL){
      printf("%s", argv[1]);
      error("Cannot open file");
      exit(1);
    }
    int count = 1;
    while (!feof(fp)){
      t = parsething(fp);
      thing ret = eval(&t, &rootenv);
      printf("%d :", count);
      print(&ret, stdout);
      printf("\n");
      count ++;
    }
    /* thing *this = t.list.first; */
    /* printf("%s = %s or %d ", this->thing.string, this->thing.data, *((int *) this->thing.data)); */
    /* print(&t, stdout); */
    /* printf("\n"); */

    /* printf("\n"); */
    return 0;
  }

  error("No file supplied");
  return 1;
}
