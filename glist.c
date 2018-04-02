
void *glistFind(void *data, glist* list){
  while(1){
    if (list->first == NULL)
      return NULL;
    if (list->first == data)
      return list;
    list = list->rest;
    if (list == NULL)
      return NULL;
  }
}

void *glistPush(void *data, glist* list){
  glist* item = malloc(sizeof(glist));
  if (list->first == NULL && list->rest == NULL){
    // if empty cell
    list->first = data;
    return;
  }
  // copy first cell to new cell
  item->first = list->first;
  item->rest = list->rest;
  // replace data in first cell
  list->first = data;
  list->rest = item;
}

#define glistPop(list) (free(list), list=list->rest, list->first)
/* void *glistPop(glist* list){ */
/*   void *data = list->first; */
/*   list-> */
/* } */
