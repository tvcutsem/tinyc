#include "type.h"

#include <stdexcept>

string type2str(Type type) {
   switch(type) {
     case int_t: return string("int");
     case char_t: return string("char");
     case string_t: return string("string");
     case fun_t: return string("function");
     case array_t: return string("array");
     case void_t: return string("void");
     default: throw logic_error("variable type unknown");
   }
}

//the only conversion supported by Tiny is an implicit
//conversion from char to int, or int to char, every other type is strict
bool coerces_to(Type t1, Type t2) {
   if (t1 == t2)
      return true;
   if ((t1 == char_t) && (t2 == int_t))
      return true;
   if ((t1 == int_t) && (t2 == char_t))
      return true;
   else
      return false;
}
