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

void addVar(char* name, thing t, glist *list){
  twothings *ff = malloc(sizeof(twothings));
  ff->first.type = TSYM;
  ff->first.data = name;
  ff->second = t;
  glistPush(ff, list);
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
  addCFunc("first", first, &rootenv.vars);
  addCFunc("rest", rest, &rootenv.vars);
  addCFunc("print", printF, &rootenv.vars);
  addCFunc("include", include, &rootenv.vars);
  addCFunc("push", push, &rootenv.vars);
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
      print(t, stdout);
      return NIL;
    }
    return tt->second;
  }
  else if (t->type == TLIST){ 
    // first thing of list is a function or a macro or a special function
    glist *list = t->data;
    if (list->first == NULL)
      return *t;
    thing *first = list->first;
    if (first->type == TSYM){
      if (strcmp(first->data, "quote") == 0){
	if (list->rest == NULL){
	  return NIL;
	}
	return *(thing *)list->rest->first;
      }
      
      if (strcmp(first->data, "if") == 0){
	// special cases
	if (list->rest == NULL || list->rest->rest == NULL){
	  error("Useless if not allowed");
	  return NIL;
	}
	thing arg1 = eval(list->rest->first, env);
	if (arg1.type == TSYM && strcmp(arg1.data, "NIL") == 0){
	  if (list->rest->rest->rest == NULL){
	    return NIL;
	  }
	  thing arg3 = eval(list->rest->rest->rest->first, env);
	  return arg3;
	}
	thing arg2 = eval(list->rest->rest->first, env);
	return arg2;
      }
      
      if (strcmp(first->data, "fun") == 0){
	// TODO: FUNCTION SHOULD BE ABLE TO FORM CLOSURES
	/* thing arg1 = eval(t->list.rest->first, env); //function name */
	func *function = malloc(sizeof(func));   //function object (internal representation)
	function->args = *((glist *)((thing *)list->rest->first)->data);
	function->body.type = TLIST;
	function->body.data = list->rest->rest;
	thing fthing = {TFUN, function};
	return fthing;
      }

      if (strcmp(first->data, "macro") == 0){
	// Macro get their arguments unevaluated
	// and return code to be evaluated 
	
	// TODO: MACRO ARGUMENT EXPANSION SHOULD BE DONE (DON'T DO IT NEXT APPROACH)
	// OTHERWISE ARGUMENT NAME CLASH IN NESTED MACROS IS HIGHLY PROBABLE
	/* thing arg1 = eval(t->list.rest->first, env); //function name */
	func *function = malloc(sizeof(func));   //function object (internal representation)
	function->args = *((glist *)((thing *)list->rest->first)->data);
	function->body.type = TLIST;
	function->body.data = list->rest->rest;
	thing fthing = {TMACRO, function};
	return fthing;
      }

      if (strcmp(first->data, "eval") == 0){
	if (list->rest == NULL){
	  return NIL;
	} else {
	  thing tt = eval(list->rest->first, env);
	  thing ttt = eval(&tt, env);
	  return ttt;
	}
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

      
      if (strcmp(first->data, "set") == 0){
	thing arg1;
	if (((thing *)list->rest->first)->type != TSYM)
	  arg1 = eval(list->rest->first, env);
	else
	  arg1 = *(thing *)list->rest->first;

	if (arg1.type != TSYM){
	  error("set name not a symbol");
	  return NIL;
	}
	
	/* twothings *entry = (twothings *)malloc(sizeof(twothings)); */
	twothings *entry = findVar(arg1, &env->vars);
	if (entry == NULL){
	  error("Variable not defined");
	  return NIL;
	}
	entry->second = eval(list->rest->rest->first, env); // value
	return entry->second;
      }
    }

    // Default case, first item is a function
    // could have directly evaluated but for efficiency
   
    thing funcThing;
    if (first->type == TSYM){
      twothings *tt = findVar(*first, &env->vars);
      if (tt == NULL){
	printf("%s is ", first->data);
	error("Unknown symbol ");
	return NIL;
      }
      if (tt->second.type != TFUN && tt->second.type != TCFUN && tt->second.type != TMACRO){
	error("Symbol not a function or a macro");
	print(&tt->second, stdout);
	return NIL;
      }
      funcThing = tt->second;
      /* f = (func *)tt->second.data; */
    } else if (first->type == TLIST){
      thing aeval = eval(first, env); // after evaluation
      if (aeval.type != TFUN && aeval.type != TCFUN && aeval.type != TMACRO){
	error("First thing should evaluate to a function or macro but got:" );
	print(&aeval, stdout);
	printf("\n");
	return NIL;
      }
      /* f = (func *)aeval.data; */
      funcThing = aeval;
    } else {
      error("First thing not a function got");
      print(first, stdout);
      return NIL;
    }

    if (funcThing.type == TCFUN){
      //thing (*function)(glist* p,env* q);
      thing (*function)(glist*,struct env*);
      function = funcThing.data;
      thing rVal = function(list->rest, env);
      return rVal;
    }

    // IF MACRO or a FUNCTION
    func *f = (func *) funcThing.data;
    int restArg = 0;
    // Push arguments to environment
    glist *rargs = &f->args; // required args
    glist *gargs = list->rest; // given args
    glist argVars = {NULL, NULL}; 
    while(1){
      if (rargs == NULL || rargs->first == NULL)
	break;
      if (gargs == NULL || gargs->first == NULL){
	error("Arguments insufficient for");
	print(&funcThing, stdout);
	return NIL;
      }
      thing *rarg = (thing *)rargs->first;
      thing *garg = (thing *)gargs->first;

      twothings *argVar = malloc(sizeof(twothings));

      argVar->first = *rarg;

      if (argVar->first.type == TLIST){
	argVar->first = eval(&argVar->first, env);
      }
      if (argVar->first.type != TSYM){
      	/* TODO: this checking should be done when func is evaled */ // no no 
      	error("Argument Variable not a symbol");
      	return NIL;
      }

      if (funcThing.type == TFUN){
	argVar->second = eval(garg, env); // arguments are evaled for a function
      }
      else if (funcThing.type == TMACRO) {
	if (restArg){
	  thing restArgs = {TLIST, gargs};
	  argVar->second = restArgs;

	  glistPush(argVar, &env->vars);
	  glistPush(argVar, &argVars);
	  break;
	}
	argVar->second = *garg; // while they are not evaled for a macro
	
	if (strcmp(argVar->first.data, ":rest") == 0){
	  restArg = 1;
	  rargs = rargs->rest;
	  continue;
	}
      }
      glistPush(argVar, &env->vars);
      glistPush(argVar, &argVars);

      if (rargs->rest == NULL)
	break;
      if (gargs->rest == NULL){
	error("Arguments insufficient for ");
	print(&funcThing, stdout);
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
    /* /\* TODO: this may be after the args are removed from env *\/ // done */
    /* if (funcThing.type == TMACRO){ */
    /*   // result of macro is evaluated */
    /*   rVal = eval(&rVal, env); */
    /* } */
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
    fprintf(f, "%d", (*(int *)t->data));
    break;
  case TSYM:
    fprintf(f, "%s:SYM", t->data);
    break;
  case TSTR:
    fprintf(f, "%s", t->data);
    break;
  case TMACRO:
  case TFUN:
    fprintf(f, "(-%p-):%s",t->data,(t->type == TFUN)?"FUN":"MACRO");
    fprintf(f, "\n\tArgs: ");
    fflush(f);
    thing temp = {TLIST, &((func *)t->data)->args};
    print(&temp, f);
    fflush(f);
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

void openFileAndEval(char *name){
  FILE *fp;
  thing t;
  if ((fp = fopen(name, "r")) == NULL){
      printf("%s", name);
      error("Cannot open file");
      exit(1);
    }
    /* int count = 1; */
    while (!feof(fp)){
      t = parsething(fp);
      thing ret = eval(&t, &rootenv);
      /* printf("%d: ", count); */
      /* print(&ret, stdout); */
      /* printf("\n"); */
      /* count ++; */
    }
}

int main(int argc, char **argv){
  initenv();
  if (argc == 2){
    openFileAndEval(argv[1]);
    return 0;
  }

  error("No file supplied");
  return 1;
}
