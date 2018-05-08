#include "stdlibrary.h"

twothings *binaryIntFunc(glist *args, env *e){
  static twothings ret;
  if (args == NULL){
    error("Args is NULL");
    exit(1);
  } 
  if (args->first == NULL){
    error("No arguments given");
    return NULL;
  }
  thing a = eval(args->first, e);
  if (args->rest == NULL || args->rest->first == NULL){
    error("Two arguments required");
    return NULL;
  }
  thing b = eval(args->rest->first, e);
  if (a.type != TINT){
    error("Arg1 doesn't eval to an integer");
    return NULL;
  }
  if (b.type != TINT){
    error("Arg2 doesn't eval to an integer");
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

thing first(glist *args, env *e){
  if (args->first == NULL){
    error("No arguments given to first");
    return NIL;
  }
  thing f = eval(args->first, e);
  if (f.type != TLIST){
    error("First argument is not a list");
    return NIL;
  }
  return *((thing *)((glist*)f.data)->first);
}

thing rest(glist *args, env *e){
  if (args->first == NULL){
    error("No arguments given to rest");
    return NIL;
  }
  thing f = eval(args->first, e);
  if (f.type != TLIST){
    error("First argument is not a list");
    return NIL;
  }
  if (((glist *) f.data)->rest == NULL)
    return NIL;
  thing res = {TLIST, ((glist*)f.data)->rest};
  return res;
}

thing push(glist *args, env *e){
  if (args->first == NULL){
    error("No arguments given");
    return NIL;
  }
  if (args->rest == NULL || args->rest->first == NULL){
    error("Two arguments required, only one given.");
    return NIL;
  }

  /* thing *list = malloc(sizeof(thing)); */
  thing list = eval((thing *)args->rest->first, e);
  
  if (list.type != TLIST){
    error("Second argument not a list");
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

thing printF(glist *args, env *e){
  if (args->first == NULL){
    error("No arguments given to print");
    return NIL;
  }
  thing tt = eval(args->first, e);
  print(&tt, stdout);
  return tt;
}

thing include(glist *args, env *e){
  if (args->first == NULL){
    error("No file given to include");
    return NIL;
  }
  thing tt = eval(args->first, e);
  if (tt.type != TSTR){
    error("Filename not a string");
    return NIL;
  }
  parseAndEval(tt.data);
  return NIL;
}
