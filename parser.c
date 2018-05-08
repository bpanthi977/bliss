#include "parser.h"

int isInteger(char *s){
  if (*s == '-' && *(s+1) != '\0')
    s++;
  while (*s!='\0'){
    if (!isdigit(*s))
      return 0;
    s++;
  }
  return 1;
}



char *getNonListString (FILE *f){
  static char string[100];
  int c;
  int pos = 0;
  thing t;
  string[0] = '\0';
  int instring = 0;
  /* printf("\nPa\n"); */
  while((c = getc(f)) != EOF){
    /* printf("-%c-\n", c); */
    if (instring){
      string[pos++] = c;
      string[pos] = '\0';
      if (c == '"')
	return string;
      continue;
    }
    if (c == '"' && !instring)
      instring = 1;
 
    
    switch (c){
    case ' ':
    case '\n':
    case '\r':
    case '\t':
      if (pos != 0){
	return string;
      }
      break;
    case '(' :
      ungetc('(', f);
      return string;
    case ')':
      ungetc(')', f);
      return string;      
    default :
      string[pos++] = c;
      string[pos] = '\0';
      break;
    }
  }
  return string;
}

thing parseNonList(FILE *f){
  thing t;
  char *str = getNonListString(f);
  // currently either integer, string or symbol
  if (str[0] == '"'){    
    //string
    t.type = TSTR;
    int len = strlen(str);
    /* printf("String %s length = %d ", str, len); */
    char *data = malloc(len -1);
    strncpy(data, str+1, len - 2);
    t.data = data;
  } else if (isInteger(str)){
    // integer
    t.type = TINT;
    int *data = malloc(sizeof(int));
    *data = (int) atoi(str);
    t.data = data;
  } else {
    // symbol
    // no fancy readtable now 
    t.type = TSYM;
    /* void *data = findSymbol(a.string, env); */
    int len = strlen(str);
    char *data = malloc(len);
    strcpy(data, str);
    t.data = data;
    /* t.data = str; */
    /* printf("New symbol: %s\n", t.data); */
  }
  return t;
}

thing parsething(FILE *f){
  int c;
  int inlist = 0;
  int infirstcell = 0;
  int normalend = 1;
  thing t;
  glist *curcell;

  while ((c = getc(f)) != EOF){
    normalend = 0;
    switch (c){
    case ' ':
    case '\n':
    case '\t':
    case '\r':
      normalend = 1;
      break;
    case ')':
      if (!inlist){
	error("Unexpected symbol ')' ", NULL);
	return t;
      } else {
	return t;
      }

    case '(':
      if (!inlist){
	inlist = 1;
	infirstcell = 1;
	t.type = TLIST;
	curcell = malloc(sizeof(glist));
	curcell->first = NULL;
	curcell->rest = NULL;
	t.data = curcell;
	break;
      }
      // else fall through
    default:
      ungetc(c,f);
      if (inlist){
	// allocate memory for the thing
	thing *this = malloc(sizeof(thing));
	*this = parsething(f);
	// no need to create new cell for fist one
	if (infirstcell)
	  infirstcell = 0;
	else {
	  glist *cell = malloc(sizeof(glist));
	  curcell->rest = cell;
	  curcell = cell;
	}
	// add data to last cell
	curcell->first = this;
	curcell->rest = NULL;
      } else {
	t = parseNonList(f);
	return t;
      }
      break;
    }
  }
  if (normalend)
    return NIL;
  
  if (feof(f)){
    error("File ended while parsing", NULL);
  } 
  if (ferror(f)){
    perror(f);
  }
  return NIL;
}




void parseAndEval(char *name){
  FILE *fp;
  thing t;
  if ((fp = fopen(name, "r")) == NULL){
      printf("%s", name);
      thing t = {TSTR, name};
      error("Cannot open file: ", &t);
      exit(1);
    }
    while (!feof(fp)){
      t = parsething(fp);
      thing ret = eval(&t, &rootenv);
    }
}

