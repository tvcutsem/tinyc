#ifndef TYPE_H
#define TYPE_H

#include <string>

using namespace std;

enum Type {
   int_t,
   char_t,
   string_t,
   fun_t,
   array_t,
   void_t
};

/**
 * Returns a printable name for a type.
 */
string type2str(Type t);

/**
 * Return whether type 1 can be coerced to type 2 without any problem.
 */
bool coerces_to(Type t1, Type t2);

#endif
