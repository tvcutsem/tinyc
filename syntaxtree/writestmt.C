#include "writestmt.h"

#include <sstream>

void WriteStatement::setExpression(Expression* newExp) {
  if (exp_ == newExp)
    return;
  delete exp_;
  exp_ = newExp;
}

ostream& WriteStatement::prettyPrint(ostream& os,int indent) const {
   while (indent--)
      os << " ";
   os << "write ";
   exp_->prettyPrint(os);
   return os;
}

string WriteStatement::toString() const {
   stringstream ss;
   ss << "[ Write [" << exp_->toString() << "]]";
   return ss.str();
}
