#include "eval.h"
int specialForm;

thing evalSymbol(thing *t, env *env){
  // Return symbols value
  twothings* tt = findVar(*t, &env->vars);
  if (tt == NULL){
    error("Symbol value unknown");
    print(t, stdout);
    return NIL;
  }
  return tt->second;
}

thing evalSpecialForm(glist *list, thing *first, env *env){
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
      return eval(list->rest->rest->rest->first, env); // arg3
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

    func *function = malloc(sizeof(func));   //function object (internal representation)
    function->args = *((glist *)((thing *)list->rest->first)->data);
    function->body.type = TLIST;
    function->body.data = list->rest->rest;
    thing fthing = {TMACRO, function};
    /* thing result = eval(&fthing, env); */
    /* return result; */

    return fthing;
  }

  if (strcmp(first->data, "eval") == 0){
    if (list->rest == NULL){
      removeDebug();
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
	
    twothings *entry = findVar(arg1, &env->vars);
    if (entry == NULL){
      error("Variable not defined");
      return NIL;
    }
    entry->second = eval(list->rest->rest->first, env); // value
    return entry->second;
  }

  // not a special form
  specialForm = 0;
}


thing evalFuncOrMacro(glist *list, thing *first, env *env){
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
  /* TODO: this may be after the args are removed from env */ // done
  if (funcThing.type == TMACRO){
    // result of macro is evaluated
    rVal = eval(&rVal, env);
  }
  return rVal;
}

thing eval(thing *t, env *env){
  addDebug(t);
  thing result;
  if (t->type == TSYM){
    result = evalSymbol(t, env);
  }
  
  else if (t->type == TLIST){ 
    // first thing of list is a function or a macro or a special function
    glist *list = t->data;
    if (list->first == NULL){
      removeDebug();
      return *t;
    }
    thing *first = list->first;

    if (first->type == TSYM){
      specialForm = 1;
      result = evalSpecialForm(list, first, env);
    }
    if (specialForm == 0){
      result = evalFuncOrMacro(list, first, env);
    }
  }
  else {
    removeDebug();
    return *t;
  }
  removeDebug();
  return result;  
}
