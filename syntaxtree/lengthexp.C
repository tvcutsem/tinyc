#include "lengthexp.h"

#include <sstream>

LengthExpression::~LengthExpression() {
   delete lexp_;
}

void LengthExpression::setLExpression(LExpression* newExp) {
  if (lexp_ == newExp)
    return;
  delete lexp_;
  lexp_ = newExp;
}

ostream& LengthExpression::prettyPrint(ostream& os,int indent) const {
   while (indent--)
      os << " ";
   os << "length ";
   lexp_->prettyPrint(os);
   return os;
}

string LengthExpression::toString() const {
   stringstream ss;
   ss << "[ Length [" << lexp_->toString() << "]]";
   return ss.str();
}
