#include "vector.h"
#include "common_debug.h"
#include <stdlib.h>

vector* vector_createNew(int estimatedLen, int stride) {
  assert(estimatedLen > 0);
  assert(stride > 0);

  // create vector
  vector* out = malloc(sizeof(vector));
  out->len = 0;
  out->mlen = estimatedLen;
  out->stride = stride;
  
  // create buffer held by vector
  void* buf = malloc(stride * estimatedLen);
  // set to vector
  out->buffer = buf;

  return out;
}

void vector_add(vector* v, void* d) {
  assert(v != NULL);
  assert(d != NULL);

  // if there is still space left to fill
  if (v->len < v->mlen) {
    *(v->buffer + v->len) = d;
    v->len++;
  }
  // otherwise it need to allocate more space
  // only allocate more by 1 element
  else {
    v->buffer = realloc(v->buffer, (v->mlen + 1)*v->stride); 
    v->mlen++;
    v->buffer[v->len] = d;
    v->len++;
  }
}

void vector_remove(vector* v, int i) {
  assert(v != NULL);
  assert(i < v->len);

  // shift element as one has been removed
  for (int index=i; index<v->len-1; index++) {
    v->buffer[index] = v->buffer[index+1];
  }

  // set null to last element
  v->buffer[v->len-1] = NULL;

  // update len
  // note: don't decrement mlen, we will only grow not shrink.
  // only shrink when free via vector_free()
  v->len--;
}

void* vector_get(vector* v, int i) {
  assert(v != NULL);
  assert(i < v->len);

  return v->buffer[i];
}

void vector_free(vector* v) {
  assert(v != NULL);

  // free its buffer first
  if (v->buffer != NULL) {
    free(v->buffer);
    v->buffer = NULL;
  }

  // free the source
  free(v);
  v = NULL;
}
