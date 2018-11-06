// test code for vectorDot
#include <stdio.h>
#include "vectorDot.h"
#include "Dot.h"

vectorDot* vDot;

void printElements(vectorDot* v) {
  printf("struct info: len=>%d, mlen=>%d\n", v->len, v->mlen);
  for (int i=0; i<v->len; i++) {
    printf("element %d: %f, %f\n", i, v->buffer[i].posX, v->buffer[i].posY);
  }
}

int main(int argc, char* argv[])
{
  vDot = vectorDot_createNew(5);

  // add a new element
  Dot newDot;
  newDot.posX = 1.0;
  newDot.posY = 2.0;
  vectorDot_add(vDot, newDot);
  printElements(vDot);
  
  // add a new element
  newDot.posX = 3.0;
  newDot.posY = 4.0;
  // it's ok as passed by value
  vectorDot_add(vDot, newDot);
  printElements(vDot);

  // add a new element
  newDot.posX = 5.0;
  newDot.posY = 6.0;
  vectorDot_add(vDot, newDot);
  printElements(vDot);

  // add a new element
  newDot.posX = 7.0;
  newDot.posY = 8.0;
  vectorDot_add(vDot, newDot);
  printElements(vDot);
  
  // add a new element
  newDot.posX = 9.0;
  newDot.posY = 10.0;
  vectorDot_add(vDot, newDot);
  printElements(vDot);

  // add a new element
  newDot.posX = 11.0;
  newDot.posY = 12.0;
  vectorDot_add(vDot, newDot);
  printElements(vDot);

  // remove 2nd element
  vectorDot_remove(vDot, 1);
  printElements(vDot);

  // get before last element
  Dot elem = vectorDot_get(vDot, vDot->len-2);
  printf("pre-last element: %f,%f\n", elem.posX, elem.posY);

  // free vDot
  vectorDot_free(vDot);

  return 0;
}
