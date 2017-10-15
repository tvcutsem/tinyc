#include "parami.h"

ParamI::ParamI(VarRegister* val): value_(val) { }

ParamI::~ParamI() {
  delete value_;
}

string ParamI::toString() const {
  return "PARAM "+ *(value_->name());
}

VarRegister* ParamI::getValue() const {
  return value_;
}

void ParamI::setValue(VarRegister* newVal) {
  delete value_;
  value_ = newVal;
}
