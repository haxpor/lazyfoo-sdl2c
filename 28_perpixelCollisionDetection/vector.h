/*
 * Equivalent general-purpose vector in C.
 * Note that caller needs to know exactly the struct definition to get/set via this vector.
 */

#ifndef vector_h_
#define vector_h_

// our vector type
struct vector
{
  /// number of element 
  int len;

  // (internal use only) managed length of estimated legnth
  int mlen;

  // (internal use only) size of element in bytes
  // it'll be set only via vector_createNew()
  int stride;

  /// buffer pointer to hold all data
  void** buffer;
};
typedef struct vector vector;

// create a new vector
//
// estimated_len - length of estimated element to be added in the future use
// stride - size of individual element
extern vector* vector_createNew(int estimatedLen, int stride);

// add a new element to vector
//
// vector - vector pointer
// d - General purpose data to be added into vector
extern void vector_add(vector* v, void* d);

// remove element at index i
//
// vector - vector pointer
// i - index to be removed
extern void vector_remove(vector* v, int i);

// get element from vector at specified index
//
// vector - vector pointer
// i - index to get vector element
//
// return general purpose data
extern void* vector_get(vector* v, int i);

// free vector from memory
// after calling this function, vector will be freed and set to NULL.
extern void vector_free(vector*);

#endif /* vector_h_ */
