typedef struct thing thing;
typedef struct list list;
typedef struct atom atom;

struct atom {
  int type;
  char string[20];
  void *data;
};

struct list {
  thing *first;
  struct list *rest;  
};

enum tatom {TINT, TSTR, TSYM, TFUN};
enum tthing { TATOM, TLIST};

struct thing {
  enum tthing type;
  union {
    atom atom;
    list list;
  }
};

struct glist{
  void *first;
  struct glist *rest;
};
typedef struct glist glist;

typedef struct {
  atom symbol;
  atom value;
} var;

typedef struct{
  atom symbol;
  glist args;
  int argc;
  atom value;
} func ;
  
struct env{
  glist vars;
  glist funcs;
};
typedef struct env env;

thing NIL = {TATOM, {0, "NIL"}};


