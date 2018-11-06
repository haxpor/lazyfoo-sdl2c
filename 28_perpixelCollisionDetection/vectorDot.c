#include "vectorDot.h"
#include "Dot.h"
#include "common_debug.h"
#include <stdlib.h>

vectorDot* vectorDot_createNew(int estimatedLen) {
  assert(estimatedLen > 0);

  // create vectorDot
  vectorDot* out = malloc(sizeof(vectorDot));
  out->len = 0;
  out->mlen = estimatedLen;
  
  // create buffer held by vectorDot
  Dot* buf = malloc(sizeof(Dot) * estimatedLen);
  // set to vectorDot
  out->buffer = buf;

  return out;
}

void vectorDot_add(vectorDot* v, Dot d) {
  assert(v != NULL);

  // if there is still space left to fill
  if (v->len < v->mlen) {
    *(v->buffer + v->len) = d;
    v->len++;
  }
  // otherwise it need to allocate more space
  // only allocate more by 1 element
  else {
    v->buffer = realloc(v->buffer, (v->mlen + 1)*sizeof(Dot)); 
    v->mlen++;
    v->buffer[v->len] = d;
    v->len++;
  }
}

void vectorDot_remove(vectorDot* v, int i) {
  assert(v != NULL);
  assert(i < v->len);

  // shift element as one has been removed
  for (int index=i; index<v->len-1; index++) {
    v->buffer[index] = v->buffer[index+1];
  }

  // update len
  // note: don't decrement mlen, we will only grow not shrink.
  // only shrink when free via vectorDot_free()
  v->len--;
}

Dot vectorDot_get(vectorDot* v, int i) {
  assert(v != NULL);
  assert(i < v->len);

  return v->buffer[i];
}

void vectorDot_free(vectorDot* v) {
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
