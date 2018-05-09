#include "stdlibrary.h"

twothings *binaryIntFunc(glist *args, env *e){
  static twothings ret;
  if (args == NULL){
    error("Args is NULL", NULL);
    exit(1);
  } 
  if (args->first == NULL){
    error("No arguments given", NULL);
    return NULL;
  }
  thing a = eval(args->first, e);
  if (args->rest == NULL || args->rest->first == NULL){
    error("Two arguments required", NULL);
    return NULL;
  }
  thing b = eval(args->rest->first, e);
  if (a.type != TINT){
    error("Arg1 doesn't eval to an integer", &a);
    return NULL;
  }
  if (b.type != TINT){
    error("Arg2 doesn't eval to an integer", &b);
    return NULL;
  }
  ret.first = a;
  ret.second = b;
  return &ret;
}

thing add(glist *args, env *e){
  twothings *tt = binaryIntFunc(args,e);
  if (tt == NULL)
    return NIL;
  
  int *sum = malloc(sizeof(int));
  *sum = *(int *)tt->first.data + *(int *)tt->second.data;
  thing ans = {TINT, sum};
  return ans;
}


thing sub(glist *args, env *e){
  twothings *tt = binaryIntFunc(args,e);
  if (tt == NULL)
    return NIL;
  
  int *dif = malloc(sizeof(int));
  *dif = *(int *)tt->first.data - *(int *)tt->second.data;
  thing ans = {TINT, dif};
  return ans;
}


thing multiply(glist *args, env *e){
  twothings *tt = binaryIntFunc(args,e);
  if (tt == NULL)
    return NIL;
  
  int *sum = malloc(sizeof(int));
  *sum = *(int *)tt->first.data * *(int *)tt->second.data;
  thing ans = {TINT, sum};
  return ans;
}


thing divide(glist *args, env *e){
  twothings *tt = binaryIntFunc(args,e);
  if (tt == NULL)
    return NIL;
  
  int *sum = malloc(sizeof(int));
  int *b = (int *)tt->second.data;
  if (*b == 0){
    error("Division by zero not possible", NULL);
    *sum = 0;
  } else {
    *sum = *(int *)tt->first.data / *b;
  }
  thing ans = {TINT, sum};
  return ans;
}


thing areEql(glist *args, env *e){
  twothings *tt = binaryIntFunc(args,e);
  if (tt == NULL)
    return NIL;
  
  int *sum = malloc(sizeof(int));
  if (*(int *)tt->first.data ==  *(int *)tt->second.data)
    return T;
  else
    return NIL;
}

thing isGt(glist *args, env *e){
  twothings *tt = binaryIntFunc(args,e);
  if (tt == NULL)
    return NIL;
  
  int *sum = malloc(sizeof(int));
  if (*(int *)tt->first.data >  *(int *)tt->second.data)
    return T;
  else
    return NIL;
}



thing eval_each(glist *args, env *env){
  if (args == NULL || args->first == NULL){
    error("No arguments given to eval-each", NULL);
    return NIL;
  }
  thing first = eval(args->first, env);
  if (first.type != TLIST){
    error("First argument doesn't evaluate to list", &first);
    return NIL;
  }
  return evalEach(first.data, env);  
}

thing create_list(glist *args, env *e){
  glist *l = malloc(sizeof(glist));
  glist *list = l;
  l->first = NULL;
  l->rest = NULL;
  while (args != NULL && args->first != NULL){
    thing *t = malloc(sizeof(thing));
    *t = eval(args->first, e);
    if (l->first == NULL)
      l->first = t;
    else {
      glist *ll  = malloc(sizeof(glist));
      l->rest = ll;
      ll->first = t;
      l = ll;
    }
    args = args->rest;
  }
  thing listThing = {TLIST, list};
  return listThing;
}

thing first(glist *args, env *e){
  if (args->first == NULL){
    error("No arguments given to first", NULL);
    return NIL;
  }
  thing f = eval(args->first, e);
  if (f.type != TLIST){
    error("First argument is not a list", NULL);
    return NIL;
  }
  return *((thing *)((glist*)f.data)->first);
}

thing rest(glist *args, env *e){
  if (args->first == NULL){
    error("No arguments given to rest", NULL);
    return NIL;
  }
  thing f = eval(args->first, e);
  if (f.type != TLIST){
    error("First argument is not a list", &f);
    return NIL;
  }
  if (((glist *) f.data)->rest == NULL)
    return NIL;
  thing res = {TLIST, ((glist*)f.data)->rest};
  return res;
}

thing push(glist *args, env *e){
  if (args->first == NULL){
    error("No arguments given", NULL);
    return NIL;
  }
  if (args->rest == NULL || args->rest->first == NULL){
    error("Two arguments required, only one given.", NULL);
    return NIL;
  }

  /* thing *list = malloc(sizeof(thing)); */
  thing list = eval((thing *)args->rest->first, e);
  
  if (list.type != TLIST){
    error("Second argument not a list", &list);
    return NIL;
  }
  
  glist *newlist = malloc(sizeof(glist));
  thing *first = malloc(sizeof(thing));
  *first = eval(args->first, e);

  newlist->first = first;
  if (((glist *)list.data)->first == NULL){
    newlist->rest = NULL;
  } else {
    newlist->rest = (glist *)list.data;
  }
  thing t = {TLIST, newlist};
  return t;
}

thing let(glist *args, env *e){
  if (args->first == NULL){
    error ("No arguments given to let", NULL);
    return NIL;
  }
 
  thing *first = (thing *)args->first;

  if (first->type != TLIST){
    error ("Variable list not a list", NULL);
    return NIL;
  }
  glist *vlist = (glist *)first->data;
  glist var_vals = {NULL, NULL};
  // Push variables to stack
  while (vlist != NULL && vlist->first != NULL){
    thing *this = (thing *)vlist->first;
    if (this->type == TLIST){
      glist *thisl = (glist *)this->data;
      thing *first = thisl->first;
      if (first->type != TSYM){
	error("Variable name not a symbol", first);
	return NIL;
      }
      if (thisl->rest == NULL || thisl->rest->first == NULL){
	error("Value not provided for variable", first);
	return NIL;
      }
      thing val =  eval(thisl->rest->first, e);
      glistPush(addVar_Val2(*first, val, e), &var_vals);
      
    } else  if (this->type != TSYM){
      error("Variable name not a symbol", this);
      return NIL;
      
    } else {
      glistPush(addVar_Val2(*this, NIL, e), &var_vals);
    }
    
    vlist = vlist->rest;
  }
  // Evaluate body
  thing result = evalEach(args->rest, e);
  
  // Remove variables from stack
  removeArguments(&var_vals, e);

  return result;
}

thing printF(glist *args, env *e){
  if (args->first == NULL){
    error("No arguments given to print", NULL);
    return NIL;
  }
  thing tt = eval(args->first, e);
  if (args->rest != NULL){
    thing *second = (thing *) args->rest->first;
    if (second->type == TSYM && strcmp(second->data, "NIL") == 0 && tt.type == TSTR){
      printf("%s", tt.data);
      return tt;
    }
    if (second->type == TSYM && strcmp(second->data, "T") == 0){
      print(&tt, stdout);
      printf("\n");
      return tt;
    }
  }

  print(&tt, stdout);
  return tt;
}

thing include(glist *args, env *e){
  if (args->first == NULL){
    error("No file given to include", NULL);
    return NIL;
  }
  thing tt = eval(args->first, e);
  if (tt.type != TSTR){
    error("Filename not a string", &tt);
    return NIL;
  }
  parseAndEval(tt.data);
  return NIL;
}
