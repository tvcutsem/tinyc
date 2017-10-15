#include <sstream>

#include "vardecl.h"

ostream& VariableDec::prettyPrint(ostream& os,int indent) const {
   while (indent--)
      os << " ";
   os << *name_ << ":" << type2str(type_);
   return os;
}

string VariableDec::toString() const {
   stringstream s;
   s << *name_ << ":" << type2str(type_);
   return s.str();
}

Variable* VariableDec::reference() const {
  return const_cast<VariableDec*>(this);
}
