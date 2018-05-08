#include "eval.h"
int specialForm;

glist *scan4vars_vars;
int scanForVars1(thing *t){
  if (t->type == TLIST){
    scanForVars0(t->data);
  } else if (t->type == TSYM){
    glistPush(t, scan4vars_vars);
  }
  return 1;
}

void dprintList(glist *list){
  thing t = {TLIST, list};
  print(&t, stdout);
}

void scanForVars0(glist *list){

  if (list->first == NULL)
    return 1;
  
  thing *f = (thing *) list->first;
  if (f->type == TSYM && strcmp(f->data, "quote") == 0)
    return 1;

  if (f->type !=TLIST && list->rest == NULL)
    return;

  if (f->type == TLIST)
    glistMap(list, scanForVars1);
  else
    glistMap(list->rest, scanForVars1);
  return 1;
}

void scanForVars(glist *list, glist *vars){
  scan4vars_vars = vars;
  scanForVars0(list);
}

thing evalSymbol(thing *t, env *env){
  // Return symbols value
  twothings* tt = findVar(*t, &env->vars);
  if (tt == NULL){
    error("Symbol value unknown:", t);
    return NIL;
  }
  return tt->second;
}

int isSym(thing *thing){
  if (thing->type != TSYM)
    return 0;
    
  return 1;
}

thing evalSpecialForm(glist *list, thing *first, env *env){
  if (strcmp(first->data, "quote") == 0){
    if (list->rest == NULL){
      return NIL;
    }
    return *(thing *)list->rest->first;
  }
      
  if (strcmp(first->data, "if") == 0){
    // special forms
    if (list->rest == NULL || list->rest->rest == NULL){
      error("Useless if not allowed (Condition and if-true not given)", NULL);
      return NIL;
    }
    thing arg1 = eval(list->rest->first, env);
    if (arg1.type == TSYM && strcmp(arg1.data, "NIL") == 0){
      if (list->rest->rest->rest == NULL){
	return NIL;
      }
      return eval(list->rest->rest->rest->first, env); // arg3
    }
    return eval(list->rest->rest->first, env); // arg2
  }
      
  if (strcmp(first->data, "fun") == 0){
    func *function = malloc(sizeof(func));   //function object (internal representation)
    function->args = *((glist *)((thing *)list->rest->first)->data);
    
    // Check if the parameters are symbols or not
    int result = glistMap(&function->args, isSym);
    if (result == 0){
      error("Parameter of the function not a symbol", NULL);
      free(function);
      return NIL;
    }

    // Create a closure
    glist vars = {NULL, NULL};    
    scanForVars(list->rest->rest, &vars);

    function->closure.first = NULL;
    function->closure.rest = NULL;
    glist *varsp = &vars;
    while(varsp != NULL && varsp->first != NULL){
      // var of parameter is not in closure
      if (findVar(*((thing *)varsp->first), &function->args) != NULL){
	varsp = varsp->rest;
	continue;
      }

      twothings *var_val = malloc(sizeof(twothings));
      twothings *vv = findVar(*((thing *)varsp->first), &env->vars);
      if (vv == NULL){
	// just skip it when unknown symbol occurs
	varsp = varsp->rest;
	continue;
      }   
	
      *var_val = *vv;
      glistPush(var_val, &function->closure);

      varsp = varsp->rest;
    }
    // Collect and return the function object
    function->body.type = TLIST;
    function->body.data = list->rest->rest;
    thing fthing = {TFUN, function};
    return fthing;
  }

  if (strcmp(first->data, "macro") == 0){
    // Macro get their arguments unevaluated
    // and return code to be evaluated 
	
    // TODO: MACRO ARGUMENT EXPANSION SHOULD BE DONE 
    // OTHERWISE ARGUMENT NAME CLASH IN NESTED MACROS IS HIGHLY PROBABLE
    // (now this is not a problem as macros are same as functions but
    //  without closure and evaluation of arguments)
    func *function = malloc(sizeof(func));   //function object (internal representation)
    function->args = *((glist *)((thing *)list->rest->first)->data);
    function->closure.first = NULL;
    function->closure.rest = NULL;
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
      return eval(&tt, env);
    }
  }

  if (strcmp(first->data, "def") == 0){
    thing arg1;
    if (((thing *)list->rest->first)->type != TSYM)
      arg1 = eval(list->rest->first, env);
    else
      arg1 = *(thing *)list->rest->first;

    if (arg1.type != TSYM){
      error("def name not a symbol", &arg1);
      return NIL;
    }

    twothings *entry = (twothings *)malloc(sizeof(twothings));
    entry->first = arg1;
    entry->second = eval(list->rest->rest->first, env); // value
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
      error("set name not a symbol", &arg1);
      return NIL;
    }
	
    twothings *entry = findVar(arg1, &env->vars);
    if (entry == NULL){
      error("Variable not defined", &arg1);
      return NIL;
    }
    entry->second = eval(list->rest->rest->first, env); // value
    return entry->second;
  }

  // not a special form
  specialForm = 0;
}

int pushArguments(thing *funcThing, glist *rargs, glist *gargs, glist *argVars, env *env){
  //                                      ||             ||
  //                               requrired args,  given args
  int len = glistLength(rargs);
  int given = glistLength(gargs);
  int restArg = 0;
 
  if (funcThing->type ==TFUN && len != given){
    error("Argument count mismatch for function", funcThing);
    return 0;
  }
  
  while(given > 0){
    given--;

    thing *rarg = (thing *)rargs->first;
    thing *garg = (thing *)gargs->first;
    thing argVal;

    if (rarg == NULL){
      error("Excess arguments given for macro", funcThing);
      return 0;
    }     
    
    if (funcThing->type == TFUN){
      // arguments are evaled for a function
      argVal= eval(garg, env); 
    }
    else if (funcThing->type == TMACRO) {
      // But they are not evaled for a macro
      argVal = *garg; 
      if (strcmp(rarg->data, ":rest") == 0){
	restArg = 1;
	rargs = rargs->rest;
	continue;
      }
      if (restArg){
	argVal.type = TLIST;
	argVal.data = gargs;
	addVar(rarg->data, argVal, &env->vars);
	glistPush(rarg, argVars);
	break;
      }
    }

    // Push variables
    addVar(rarg->data, argVal, &env->vars);
    glistPush(rarg, argVars);

    rargs = rargs->rest;
    gargs = gargs->rest;
  }
  return 1;
}

void removeArguments(glist *argVars, env *env){
  while(1){      
    if (argVars->first == NULL)
      break;
    
    /* glistRemove(argVars->first, &env->vars); */
    removeVar(((thing *)argVars->first)->data, &env->vars);
    if (argVars->rest == NULL)
      break;
    argVars = argVars->rest;      
  }
}

thing evalEach(glist *forms, env *env){
  thing rVal = NIL;
  while (1){
    if (forms == NULL || forms->first == NULL )
      return rVal;
    rVal = eval((thing *)forms->first, env);
    if (forms->rest == NULL)
      return rVal;
    forms = forms->rest;
  }
}

thing evalFuncOrMacro(glist *list, thing *first, env *env){
  // Default case, first item is a function
  // could have directly evaluated but for efficiency

  //
  // Get the function/macro
  //
  thing funcThing;
  if (first->type == TSYM){
    twothings* tt = findVar(*first, &env->vars);
    if (tt == NULL){
      error("Unknown symbol:", first);
      return NIL;
    }
    funcThing = tt->second;
    if (funcThing.type != TFUN && funcThing.type != TCFUN && funcThing.type != TMACRO){
      error("Symbol not a function or a macro", &funcThing);
      return NIL;
    }
  } else if (first->type == TLIST){
    funcThing = eval(first, env); // after evaluation
    if (funcThing.type != TFUN && funcThing.type != TCFUN && funcThing.type != TMACRO){
      error("First thing should evaluate to a function or macro but got:" , &funcThing);
      return NIL;
    }
  } else {
    error("First thing not a function got", first);
    return NIL;
  }

  //
  // If C function, pass the arguments and evaluate directly
  //
  if (funcThing.type == TCFUN){
    thing (*function)(glist*,struct env*);
    function = funcThing.data;
    thing rVal = function(list->rest, env);
    return rVal;
  }

  //
  // IF Macro or normal Function
  // Push arguments to environment
  func *f = (func *) funcThing.data;  
  glist argVars = {NULL, NULL};
  if (pushArguments(&funcThing, &f->args, list->rest, &argVars, env) == 0)
    return NIL;
  //
  // Add the closure to the environment (Only for functions. Macros have empty closures)
  glist *closr = &f->closure;
  while(closr != NULL && closr->first != NULL){
    addVar_Val(closr->first, &env->vars);
    closr = closr->rest;
  }
  //
  // Evaluate function body
  glist *forms = (glist *)f->body.data;
  thing rVal = evalEach(forms, env);

  //
  // Remove Closure and Argument vars
  closr = &f->closure;
  while(closr != NULL && closr->first != NULL){
    glistRemove(closr->first, &env->vars);
    closr = closr->rest;
  }
  removeArguments(&argVars, env);
  
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
