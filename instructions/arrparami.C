#include "arrparami.h"

ArrayParamI::ArrayParamI(ArrayRegister* val): value_(val) { }

ArrayParamI::~ArrayParamI() {
  delete value_;
}

string ArrayParamI::toString() const {
  return "ARRAYPARAM "+ *(value_->name());
}

ArrayRegister* ArrayParamI::getValue() const {
  return value_;
}

void ArrayParamI::setValue(ArrayRegister* newVal) {
  delete value_;
  value_ = newVal;
}
