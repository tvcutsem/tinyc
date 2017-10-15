#include "readstmt.h"

#include <sstream>

ReadStatement::~ReadStatement() {
  delete target_;
}

void ReadStatement::setTarget(LExpression* newTarget) {
  if (target_ == newTarget)
    return;
  delete target_;
  target_ = newTarget;
}

ostream& ReadStatement::prettyPrint(ostream& os,int indent) const {
   while (indent--)
      os << " ";
   os << "read ";
   target_->prettyPrint(os);
   return os;
}

string ReadStatement::toString() const {
   stringstream ss;
   ss << "[ Read [" << target_->toString() << "]]";
   return ss.str();
}
