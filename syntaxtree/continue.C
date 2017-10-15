#include "continue.h"

ostream& ContinueStatement::prettyPrint(ostream& os,int indent) const {
   while (indent--)
      os << " ";
   os << "continue";
   return os;
}

string ContinueStatement::toString() const {
  return string("[ Continue ]");
}

void ContinueStatement::accept(TreeVisitor* v) {
  v->visitContinueStatement(this);
}
