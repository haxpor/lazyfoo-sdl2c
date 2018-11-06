/*
 * Equivalent vector in C specifically for Dot struct.
 */

#ifndef vectorDot_h_
#define vectorDot_h_

// forward declaration for Dot used somewhere else
typedef struct Dot Dot;

// our vector type Dot
struct vectorDot
{
  /// number of element 
  int len;

  // (internal use only) managed length of estimated legnth
  int mlen;

  /// buffer pointer to hold all data
  Dot* buffer;
};
typedef struct vectorDot vectorDot;

// create a new vector
//
// estimated_len - length of estimated element to be added in the future use
extern vectorDot* vectorDot_createNew(int estimatedLen);

// add a new Dot into vector
//
// vectorDot - vectorDot pointer
// d - Dot to be added into vector
extern void vectorDot_add(vectorDot* v, Dot d);

// remove element at index i
//
// vectorDot - vectorDot pointer
// i - index to be removed
extern void vectorDot_remove(vectorDot* v, int i);

// get Dot element from vector at specified index
//
// vectorDot - vectorDot pointer
// i - index to get vectorDot element
//
// return Dot
extern Dot vectorDot_get(vectorDot* v, int i);

// free vectorDot from memory
// after calling this function, vectorDot will be freed and set to NULL.
extern void vectorDot_free(vectorDot*);

#endif /* vectorDot_h_ */
