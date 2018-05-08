#ifndef glist_h
#define glist_h
void *glistFind(void *data, glist* list);
void *glistPush(void *data, glist* list);
void glistRemove(void *data, glist *list);
int glistLength(glist *list);
int glistMap(glist *list, int func(void *));
#endif
