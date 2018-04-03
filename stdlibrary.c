
thing eval(thing *t, env *env);

thing add(glist *args, env *e){
  fflush(stdout);
  if (args == NULL){
    error("Args is NULL");
    exit(1);
  }
  if (args->first == NULL){
    error("No arguments given to +");
    return NIL;
  }
  thing a = eval(args->first, e);
  if (args->rest == NULL || args->rest->first == NULL){
    error("Two arguments required");
    return NIL;
  }
  thing b = eval(args->rest->first, e);
  if (a.type != TINT){
    error("Arg1 doesn't eval to an integer");
    return NIL;
  }
  if (b.type != TINT){
    error("Arg2 doesn't eval to an integer");
    return NIL;
  }

  int *sum = malloc(sizeof(int));

  *sum = *((int *)a.data) + *(int *)b.data;

  thing ans = {TINT, sum};
  return ans;
}
