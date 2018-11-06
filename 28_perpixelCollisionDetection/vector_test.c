// test code for vector
#include <stdio.h>
#include "vector.h"

struct myStruct {
  int integerValue;
  float floatValue;
};
typedef struct myStruct myStruct;

vector* vGeneral;

void printElements(vector* v) {
  printf("struct info: len=>%d, mlen=>%d\n", v->len, v->mlen);

  myStruct tempStruct;
  for (int i=0; i<v->len; i++) {
    // convert to myStruct
    tempStruct = *(myStruct*)(v->buffer[i]);
    printf("element %d: %d, %f\n", i, tempStruct.integerValue, tempStruct.floatValue);
  }
}

int main(int argc, char* argv[])
{
  vGeneral = vector_createNew(5, sizeof(myStruct));

  // add a new element
  {
  myStruct newStruct;
  newStruct.integerValue = 1;
  newStruct.floatValue = 2.0;
  vector_add(vGeneral, (void*)(&newStruct));
  printElements(vGeneral);
  }

  // add a new element
  {
  myStruct newStruct;
  newStruct.integerValue = 3;
  newStruct.floatValue = 4.0;
  vector_add(vGeneral, (void*)(&newStruct));
  printElements(vGeneral);
  }

  // add a new element
  {
  myStruct newStruct;
  newStruct.integerValue = 5;
  newStruct.floatValue = 6.0;
  vector_add(vGeneral, (void*)(&newStruct));
  printElements(vGeneral);
  }

  // add a new element
  {
  myStruct newStruct;
  newStruct.integerValue = 7;
  newStruct.floatValue = 8.0;
  vector_add(vGeneral, (void*)(&newStruct));
  printElements(vGeneral);
  }
  
  // add a new element
  {
  myStruct newStruct;
  newStruct.integerValue = 9;
  newStruct.floatValue = 10.0;
  vector_add(vGeneral, (void*)(&newStruct));
  printElements(vGeneral);
  }

  // add a new element
  {
  myStruct newStruct;
  newStruct.integerValue = 11;
  newStruct.floatValue = 12.0;
  vector_add(vGeneral, (void*)(&newStruct));
  printElements(vGeneral);
  }

  // remove 2nd element
  vector_remove(vGeneral, 1);
  printElements(vGeneral);

  // get before last element
  myStruct elem = *(myStruct*)vector_get(vGeneral, vGeneral->len-2);
  printf("pre-last element: %d,%f\n", elem.integerValue, elem.floatValue);

  // free vGeneral
  vector_free(vGeneral);

  return 0;
}
