#include "returnstmt.h"

#include <sstream>

void ReturnStatement::setExpression(Expression* newExp) {
  if (exp_ == newExp)
    return;
  delete exp_;
  exp_ = newExp;
}

ostream& ReturnStatement::prettyPrint(ostream& os,int indent) const {
   while (indent--)
      os << " ";
   os << "return ";
   exp_->prettyPrint(os);
   return os;
}

string ReturnStatement::toString() const {
   stringstream ss;
   ss << "[ Return [" << exp_->toString() << "]]";
   return ss.str();
}
