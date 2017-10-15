#include "binop.h"

#include <sstream>

BinaryOp::~BinaryOp() {
   delete first_;
   delete second_;
}

Type BinaryOp::type() const {
   return op_.mapsTo(first_->type(),second_->type());
}

void BinaryOp::setFirstArg(Expression* newArg) {
  if (first_ == newArg)
    return;
  delete first_;
  first_ = newArg;
}

void BinaryOp::setSecondArg(Expression* newArg) {
  if (second_ == newArg)
    return;
  delete second_;
  second_ = newArg;
}

ostream& BinaryOp::prettyPrint(ostream& os,int indent) const {
    while (indent--)
       os << " ";
    first_->prettyPrint(os);
    os << " " << op_.symbol() << " ";
    second_->prettyPrint(os);
    return os;
}

string BinaryOp::toString() const {
    stringstream s;
    s << "[" << op_.symbol() << "\n    [" << first_->toString() << " ]\n    [";
    s << second_->toString() << " ]";
    return s.str();
}
