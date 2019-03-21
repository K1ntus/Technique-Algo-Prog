#include "heap.h"
#include <stdlib.h>
#include <stdio.h>


heap heap_create(int n, int (*f)(const void *, const void *)) {
  heap res = malloc(sizeof(*res));
  void ** res_array = (void **) malloc(sizeof(void*) * (n+1));

  res->array = res_array;
  res->size = 0;
  res->nmax = n;
  res->f = f;

  return res;
}

void heap_destroy(heap h) {
  free(h->array);
  free(h);
}

bool heap_empty(heap h) {
  if(h->size > 0)
    return 1;
  return 0;
}

bool heap_add(heap h, void *object) {

  //Increment the number of object stored into the heap
  h->size += 1;

  //Check if the heap array size is great enough
  if(h->size > h->nmax){
    printf("Array too small ! Reallocating memory ...\n");
    h->nmax *= 2;
    h->array = realloc(h->array, h->nmax);  //Should use a tmp array to prevent memory corruption
  }

  //Add the object to the heap
  h->array[h->size] = object;

  //Check if his father is greater than him
  for(int i = h->size; i > 1; i=i/2){
    if(h->f(h->array[i], h->array[i/2]) < 0){ //Son is greater than his father
      void * tmp = h->array[i/2];
      h->array[i/2] = h->array[i];
      h->array[i] = tmp;
    } else {  //The object is at his place, exit the loop
      break;
    }
  }

  return 0;
}

void *heap_top(heap h) {
  return h->array[1];
}

void *heap_pop(heap h) {
  void* res_value = heap_top(h);  //get the root value to return

  h->array[1] = h->array[h->size]; //Replace the root with the latest elem
  h->size -= 1;

  int i = 1;
  while(i <= h->size){
    if(i*2 > h->size){  //Both are null, reached bottom of the tree
      break;
    }

    void * left = h->array[i*2];
    if(i*2+1 > h->size){ //Last height of the tree and left isnt empty
      if(h->f(left, h->array[i]) < 0){ //Left smaller than current node
        void * tmp = left;
        h->array[2*i] = h->array[i];
        h->array[i] = tmp;
        i = i*2+1;
      }
      break;
    }

    void * right = h->array[i*2 +1];
    if(h->f(h->array[i], h->array[2*i]) > 0 && h->f(h->array[i*2], h->array[i*2 +1]) < 0){  //right elem lower than root AND right lower than left
      void * tmp = left;
      h->array[2*i] = h->array[i];
      h->array[i] = tmp;
      i = i*2;
      continue;
    } else if(h->f(h->array[i], h->array[2*i +1]) > 0){ //Else if left lower than root
      void * tmp = right;
      h->array[2*i +1] = h->array[i];
      h->array[i] = tmp;
      i = i*2+1;
      continue;
    } else {
      break;
    }
  }

  return res_value;
}
