#ifndef dtypes_h
#define dtypes_h

// Basic datatypes
typedef struct thing thing;
typedef struct list list;
typedef struct atom atom;

struct thing {
  int type; 
  void *data;
};


enum type {TINT, TSTR, TSYM, TFUN, TCFUN, TLIST, TMACRO};

struct glist{
  void *first;
  struct glist *rest;
};
typedef struct glist glist;


typedef struct{
  glist args;
  thing body;
} func ;
  
struct env{
  glist vars;
  /* glist funcs; */
};
typedef struct env env;

typedef struct {
  thing first;
  thing second;
} twothings;

thing NIL;
thing T;
thing NEWLINE;
#endif
