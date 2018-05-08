#include "dtypes.h"
#include "glist.h"
#include "eval.h"
#include "parser.h"

thing add(glist *args, env *e);
thing sub(glist *args, env *e);
thing areEql(glist *args, env *e);
thing isGt(glist *args, env *e);
thing first(glist *args, env *e);
thing rest(glist *args, env *e);
thing push(glist *args, env *e);
thing printF(glist *args, env *e);
thing include(glist *args, env *e);
