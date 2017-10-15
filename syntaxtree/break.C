#include "break.h"

ostream& BreakStatement::prettyPrint(ostream& os,int indent) const {
   while (indent--)
      os << " ";
   os << "break";
   return os;
}

string BreakStatement::toString() const {
  return string("[ Break ]");
}

void BreakStatement::accept(TreeVisitor* v) {
  v->visitBreakStatement(this);
}
