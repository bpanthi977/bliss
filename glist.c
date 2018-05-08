// Generic list utilites
#include "dtypes.h"
#include <stdio.h>

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

void glistRemove(void *data, glist *list){
  glist *prev = NULL;
  while(1){
    if (list->first == NULL)
      break;

    if (list->first == data){
      if (prev == NULL && list->rest == NULL){
	list->first == NULL;
      }else if (prev == NULL) {
	list->first = list->rest->first;
	list->rest = list->rest->rest;	
      }
      else if (list->rest == NULL) {
	prev->rest = NULL;
      }
    }
    prev = list->first;
    if (list->rest == NULL)
      break;
    list = list->rest;
  }
}
