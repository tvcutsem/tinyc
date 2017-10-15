#include "returni.h"

ReturnI::ReturnI(VarRegister* val): value_(val) { }

ReturnI::~ReturnI() {
  delete value_;
}

string ReturnI::toString() const {
  return "RETURN "+ *(value_->name());
}

VarRegister* ReturnI::getValue() const {
  return value_;
}

void ReturnI::setValue(VarRegister* newVal) {
  delete value_;
  value_ = newVal;
}
