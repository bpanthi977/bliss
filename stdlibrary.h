#include "dtypes.h"
#include "glist.h"
#include "eval.h"
#include "parser.h"

thing add(glist *args, env *e);
thing sub(glist *args, env *e);
thing multiply(glist *args, env *e);
thing divide(glist *args, env *e);
thing areEql(glist *args, env *e);
thing isGt(glist *args, env *e);
thing eval_each(glist *args, env *env);
thing create_list(glist *args, env *e);
thing first(glist *args, env *e);
thing rest(glist *args, env *e);
thing push(glist *args, env *e);
thing let(glist *args, env *e);
thing printF(glist *args, env *e);
thing include(glist *args, env *e);
