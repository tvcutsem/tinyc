#include <sstream>

#include "unop.h"

UnaryOp::~UnaryOp() {
   delete exp_;
}

void UnaryOp::setExpression(Expression* newExp) {
  if (exp_ == newExp)
    return;
  delete exp_;
  exp_ = newExp;
}

//the second argument is a dummy type!
Type UnaryOp::type() const {
   return op_.mapsTo(exp_->type(),int_t);
}

ostream& UnaryOp::prettyPrint(ostream& os,int indent) const {
    while (indent--)
       os << " ";
    os << op_.symbol() << " ";
    exp_->prettyPrint(os);
    return os;
}

string UnaryOp::toString() const {
    stringstream s;
    s << "[" << op_.symbol() << "[" << exp_->toString() << " ]]\n";
    return s.str();
}
