#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "dtypes.h"
#include "glist.c"
/* #include "eval.c" */

void error(char message[]){
  printf("ERROR: %s\n", message);
}


void *parseatomdata(atom *a);

env rootenv;

int add(int a, int b){
  return a+b;
}

int printi(int a){
  printf("%d", a);
  return a;
}

int multi(int a, int b){
  return a*b;
}
void printvar(var a){
  printf("Var: %s = %d\n", a.symbol.data, a.value.data);
}

void printfunc(func *a){
  printf("Func: %s(..%d..)\n", a->symbol.data, a->argc);
}

void initenv(){
  rootenv.vars.first = NULL;
  rootenv.vars.rest = NULL;
  rootenv.funcs.first = NULL;
  rootenv.funcs.rest = NULL;

  atom arg1sym = {TSYM, "arg1", NULL};
  parseatomdata(&arg1sym);
  atom arg1val = {TSTR, "\"main\"", NULL};
  parseatomdata(&arg1val);
  var arg1 = {arg1sym, arg1val};
  glistPush(&arg1, &rootenv.vars);

  atom funaddsym = {TSYM, "+", NULL};
  parseatomdata(&funaddsym);
  atom funadd = {TFUN, "", add};
  glist fargs; // Not needed now
  
  /* func add = {funaddsym, fargs, 2, funadd}; */
  func *add = malloc(sizeof(func));
  add->symbol = funaddsym;
  add->args = fargs;
  add->argc = 2;
  add->value = funadd;
  glistPush(add, &rootenv.funcs);

  
  atom funmultisym = {TSYM, "*", NULL};
  parseatomdata(&funmultisym);
  atom funmulti = {TFUN, "", multi};
  glist fmargs; // Not needed now
  
  /* func add = {funaddsym, fargs, 2, funadd}; */
  func *multi = malloc(sizeof(func));
  multi->symbol = funmultisym;
  multi->args = fmargs;
  multi->argc = 2;
  multi->value = funmulti;
  glistPush(multi, &rootenv.funcs);

  
  atom printisym = {TSYM, "printi", NULL};
  parseatomdata(&printisym);
  atom funprinti = {TFUN, "", printi};
  glist fprintiargs; // Not needed now
  func printi = {printisym, fprintiargs, 1, funprinti};
  func *printif = malloc(sizeof(func));
  *printif = printi;
  glistPush(printif, &rootenv.funcs);
  /* printfunc(rootenv.funcs.rest->first); */
  /* printfunc(rootenv.funcs.first); */
}

void *findVar(char *str, glist *list){
  while(1){
    if (list->first == NULL){
      return NULL;
    }
    if(strcmp(((var*)list->first)->symbol.data, str) == 0){
      return list->first;
    }
    list = list->rest;
    if (list == NULL){
      return NULL;
    }
  }
}

void *findFunc(char *str, glist *list){
  while(1){
    if (list->first == NULL){
      return NULL;
    }
    /* printfunc(list->first); */
    char *funcSym = ((func*)list->first)->symbol.data;
    /* printf("is %s %s?", funcSym, str); */
    if(strcmp(funcSym, str) == 0){
      return list->first;
    }
    list = list->rest;
    if (list == NULL){
      return NULL;
    }
  }
}
    
thing eval(thing *t, env env){
  if (t->type == TATOM){
    if (t->atom.type == TSYM){
      var *v = (var *) findVar(t->atom.data, &env.vars);
      if (v == NULL)
	error("Undefined variable");
      thing t = {TATOM, &v->value};
      return t;
    } else {
      return *t;
    }
  } else if (t->type == TLIST){ 
    // first atom of list is a function
    thing *first = t->list.first;
    if (first->type == TATOM && first->atom.type == TSYM){
      parseatomdata(&first->atom);
      /* printf("%s", first->atom.data); */
      func *f = findFunc(first->atom.data, &env.funcs);
      int rval;
      if (f == NULL)
	error("Unknown function");
      else if (f->argc == 1){
	int (*fp)(int) = f->value.data;
	atom arg1 = eval(t->list.rest->first, env).atom;
	if (arg1.type != TINT){
	  error("Argument not integer");
	  return NIL;
	}
	rval = fp( *(int *) arg1.data);
      } else if (f->argc == 2){
	int (*fp)(int, int) = f->value.data;
	atom arg1 = eval(t->list.rest->first, env).atom;
	atom arg2 = eval(t->list.rest->rest->first, env).atom;
	if (arg1.type != TINT || arg2.type !=TINT){
	  error("Arguments not integer");
	  return NIL;
	}
	rval = fp(*(int *)arg1.data, *(int *)arg2.data);
      }else {
	error("Argument length too long");
	return NIL;
      }
      int *i = malloc(sizeof(int));
      *i= rval;
      atom val = { TINT, "", i};
      thing t = {TATOM, val};

      return t;
    } else {
      error("First symbol is not a function");
    }
      }
  else {
    error("Unknown thing");
  }
}

int isInteger(char *s){
  while (*s!='\0'){
    if (!isdigit(*s))
      return 0;
    s++;
  }
  return 1;
}

void *parseatomdata(atom *a){
  // currently either integer, string or symbol
  if (a->string[0] == '"'){    
    //string
    a->type = TSTR;
    int len = strlen(a->string);
    char *data = malloc(len -1);
    strncpy(data, a->string+1, len - 2);
    a->data = data;
  } else if (isInteger(a->string)){
    // integer
    a->type = TINT;
    int *data = malloc(sizeof(int));
    *data = (int) atoi(a->string);
    a->data = data;
  } else {
    // symbol
    // no fancy readtable now 
    a->type = TSYM;
    /* void *data = findSymbol(a.string, env); */
    int len = strlen(a->string);
    char *data = malloc(len);
    strcpy(data, a->string);
    a->data = data;
    /* a->data = a->string; */
    /* printf("New symbol: %s\n", a->data); */
  }
  return a->data;
}

atom parseatom (FILE *f){
  int c;
  int pos = 0;
  atom a;
  a.type = 0;
  a.string[0] = '\0';
  
  /* printf("\nPa\n"); */
  while((c = getc(f)) != EOF){
    /* printf("-%c-\n", c); */
    switch (c){
    case ' ':
    case '\n':
    case '\r':
    case '\t':
      if (pos != 0){
	return a;
      }
      break;
    case '(' :
      ungetc('(', f);
      return a;
    case ')':
      ungetc(')', f);
      return a;      
    default :
      a.string[pos++] = c;
      a.string[pos] = '\0';
      break;
    }
  }
  return a;
}

thing parsething(FILE *f){
  int c;
  int inlist = 0;
  int infirstcell = 0;
  thing t;
  list *curcell;
  
  /* printf("\nPt--\n"); */
  while((c = getc(f)) != EOF){
    /* printf("-%c-\n", c); */
    switch (c){

    case ' ':
    case '\n':
    case '\t':
    case '\r':
      break;
    case ')':
      if (!inlist){
	error("Unexpected symbol ')' ");
	/* printf("\n--Pt\n"); */
	return t;
      } else {
	/* printf("\n--Pt\n"); */
	return t;
      }
    case '(':
      if (!inlist){
	inlist = 1;
	infirstcell = 1;
	t.type = TLIST;
	t.list.first = &NIL;
	curcell = &t.list;
	break;
      }
      // else fall through
    default:
      ungetc(c, f);
      if (inlist){
	// allocate space for this thing 
	thing *this = malloc(sizeof(thing));
	// get it and  add it to this cell
	*this = parsething(f);

	// if on first cell no need to allocate memory
	if (infirstcell){
	  infirstcell = 0;
	} else {
	  /* printf("Newcell"); */
	  list *cell = malloc(sizeof(list));
	  // add cell to tail of last cell
	  curcell->rest = cell;
	  // change last cell to this one
	  curcell = cell;
	}
	// add data to las cell
	curcell->first = this;
	curcell->rest = NULL;
	
      } else {
	t.type = TATOM;
	t.atom = parseatom(f);
	parseatomdata(&t.atom);
	/* printf("Next char : %c\n", ungetc(getc(f), f)); */
	/* printf("Atom: "); */
	/* printf("%s = %s or %d ", t.atom.string, t.atom.data, *((int *) t.atom.data)); */
	/* print(&t); */
	/* printf("\n--Pt\n"); */
	return t;
      }
      break;
    }
  }
  if (feof(f)){
    error("File ended while parsing");
  } 
  if (ferror(f)){
    perror(f);
  }
  printf("\n--Pt\n");
}

void printatom (atom a){
  switch (a.type){
    case TINT:
      printf("%d:INT", (*(int *)a.data));
      break;
    case TSYM:
      printf("%s:SYM", a.data);
      break;
    case TSTR:
      printf("%s:STR", a.data);
      break;
    }
    /* printf("%s:%s", a.string, (a.type==TINT?"INT":a.type==TSTR?"STR":"SYM")); */
}

void print(thing *t, FILE *f){
  if (t->type == TLIST){
    putc('(', f);
    list *cell = &t->list;
    while(1) {
      print(cell->first, f);
      cell = cell->rest;
      if (cell == NULL)
	break;
      putc(' ',f);
    }
    putc(')', f);    
  } else if (t->type == TATOM){
    printatom(t->atom);
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
    t = parsething(fp);
    /* thing *this = t.list.first; */
    /* printf("%s = %s or %d ", this->atom.string, this->atom.data, *((int *) this->atom.data)); */
    /* print(&t, stdout); */
    /* printf("\n"); */
    thing ret = eval(&t, rootenv);
    printf("\n");
    return 0;
  }

  error("No file supplied");
  return 1;
}
