#include <sstream>

#include "assign.h"

Assignment::~Assignment() {
   delete target_;
   delete exp_;
}

void Assignment::setTarget(LExpression* newTarget) {
  if (target_ == newTarget)
    return;
  delete target_;
  target_ = newTarget;
}

void Assignment::setExpression(Expression* newExpression) {
  if (exp_ == newExpression)
    return;
  delete exp_;
  exp_ = newExpression;
}

ostream& Assignment::prettyPrint(ostream& os,int indent) const {
   while (indent--)
      os << " ";
   target_->prettyPrint(os);
   os << " = ";
   exp_->prettyPrint(os);
   return os;
}

string Assignment::toString() const {
   stringstream s;
   s << "[ = \n    [ " << target_->toString();
   s << " ]\n    [ " << exp_->toString() << " ]]";
   return s.str();
}
