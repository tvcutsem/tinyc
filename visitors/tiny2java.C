#include "tiny2java.h"
#include "../type.h"
#include "../syntaxtree/array.h"
#include "../syntaxtree/function.h"
#include "../instructions/varregister.h"
#include "../instructions/funregister.h"
#include "../instructions/arrayregister.h"

using namespace std;

//constants
static const string JVM_INT_T("int");
static const string JVM_CHAR_T("char");
static const string JVM_STRING_T("java.lang.String");
static const string JVM_VOID_T("void");

string type2javaType(Identifier* id) throw (std::logic_error) {
  //if id is an array, return baseType[]
  if (id->isArray()) {
    Array* a = dynamic_cast<Array*>(id);
    return tinytype2java(a->baseType())+"[]";
  }

  //if id is a function, return the returntype
  if (id->isFunction()) {
    Function* f = dynamic_cast<Function*>(id);
    return tinytype2java(f->returnType());
  }

  //for ordinary variables, use their normal type
  return tinytype2java(id->type());
}


string type2javaType(Register* reg) throw (std::logic_error) {

  //if reg is a verregister, return it's type
  VarRegister* v = dynamic_cast<VarRegister*>(reg);
  if (v)
    return tinytype2java(v->type());
  
  //if reg is an arrayregister, return baseType[]
  ArrayRegister* a = dynamic_cast<ArrayRegister*>(reg);
  if (a)
    return tinytype2java(a->baseType())+"[]";

  //if reg is a funregister, return the returntype
  FunRegister* f = dynamic_cast<FunRegister*>(reg);
  if (f)
    return tinytype2java(f->returnType());

  //for other registers, throw a logic_error
  throw std::logic_error(
      "type2javaType received erroneous register"); 
}


string tinytype2java(Type t) {
  switch (t) {
    case int_t:
      return JVM_INT_T; 
    case char_t:
      return JVM_CHAR_T;
    case string_t:
      return JVM_STRING_T;
    case void_t:
      return JVM_VOID_T;
    default:
      throw std::logic_error(
	  "Illegal typename given to tinytype2java: "+type2str(t));
  }
}
