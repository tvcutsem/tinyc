#include "calli.h"

CallI::CallI(FunRegister* fun, int numArgs):
	function_(fun),nrOfArgs_(numArgs) { }

CallI::~CallI() {
  delete function_;
}

string CallI::toString() const {
  return "CALL "+ *(function_->name());
}

FunRegister* CallI::getFunction() const {
  return function_;
}

int CallI::getNrOfArgs() const {
  return nrOfArgs_;
}

void CallI::setFunRegister(FunRegister* newFun) {
  delete function_;
  function_ = newFun;
}

void CallI::setNrOfArgs(int nrOfArgs) {
  nrOfArgs_ = nrOfArgs;
}
