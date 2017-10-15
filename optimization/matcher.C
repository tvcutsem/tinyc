#include "matcher.h"

//Register classes
#include "../instructions/numregister.h"
#include "../instructions/charregister.h"

//Operator classes
#include "../operators/divides.h"
#include "../operators/minus.h"
#include "../operators/plus.h"
#include "../operators/times.h"


bool Matcher::operator()(bool firstArg, Operator& op, Register* reg) {
  isFirst_ = firstArg;
  operatorOK_ = false;
  registerOK_ = false;
  op.accept(this);
  if (!operatorOK_)
    return false; //operator not visited, so no match
  reg->accept(this);
  if (!registerOK_)
    return false; //register not visited, no constant, no match
  //if a value was found, and it was the neutral element, then we have a match
  return (neutralElt_ == valueFound_);
}

void Matcher::visitDividesOp(DividesOp* op) {
  //if isFirst is true, then we cannot match /, ie 1 is not a neutral element
  //when used like 1 / x (not equivalent to x)
  operatorOK_ = !isFirst_;
  neutralElt_ = 1;
}

void Matcher::visitMinusOp(MinusOp* op) {
  //if isFirst is true, then we cannot match -, ie 0 is not a neutral element
  //when used like 0 - x (not equivalent to x)
  operatorOK_ = !isFirst_;
  neutralElt_ = 0;
}

void Matcher::visitPlusOp(PlusOp* op) {
  operatorOK_ = true;
  neutralElt_ = 0;
}

void Matcher::visitTimesOp(TimesOp* op) {
  operatorOK_ = true;
  neutralElt_ = 1;
}

void Matcher::visitNumRegister(NumRegister* numreg) {
  registerOK_ = true;
  valueFound_ = numreg->value();
}

void Matcher::visitQCharRegister(QCharRegister* creg) {
  registerOK_ = true;
  valueFound_ = creg->value();
}
