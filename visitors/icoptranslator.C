#include "icoptranslator.h"

//Operator classes
#include "../operators/divides.h"
#include "../operators/equal.h"
#include "../operators/evaluate.h"
#include "../operators/greater.h"
#include "../operators/less.h"
#include "../operators/minus.h"
#include "../operators/negate.h"
#include "../operators/nequal.h"
#include "../operators/operator.h"
#include "../operators/plus.h"
#include "../operators/times.h"
#include "../operators/uminus.h"
#include "../operators/operators.h"

//Instruction classes
#include "../instructions/binassigni.h"
#include "../instructions/gotoi.h"
#include "../instructions/unassigni.h"
#include "../instructions/simpleassigni.h"
#include "../instructions/charregister.h"
#include "../instructions/tempregister.h"
#include "../instructions/instructions.h"

#include "../syntaxtree/variable.h"
#include "../symboltable.h"

void ICOpTranslator::visitDividesOp(DividesOp* op) {
  generateArithMeticOp(op);
}

void ICOpTranslator::visitEqualOp(EqualOp* op) {
  generateBooleanOp(op);
}

void ICOpTranslator::visitGreaterOp(GreaterOp* op) {
  generateBooleanOp(op);
}

void ICOpTranslator::visitLessOp(LessOp* op) {
  generateBooleanOp(op);
}

void ICOpTranslator::visitMinusOp(MinusOp* op) {
  generateArithMeticOp(op);
}

void ICOpTranslator::visitNEqualOp(NEqualOp* op) {
  generateBooleanOp(op);
}

void ICOpTranslator::visitPlusOp(PlusOp* op) {
  generateArithMeticOp(op);
}

void ICOpTranslator::visitTimesOp(TimesOp* op) {
  generateArithMeticOp(op);
}

/*
 * x = !y should be translated to:
 *
 *        IF y != 0 GOTO false
 *  true: x = 1
 *        GOTO end
 * false: x = 0
 *   end: ...
 */

void ICOpTranslator::visitNegateOp(NegateOp* op) {
  //declare temporary variable to hold the result
  Variable* temp = parent_.scope_->declareTempVariable(resultType_);
  IfI* ifinstr = generateTest(&operatorNEqual,arg1_,makeZero());
  GotoI* gotoinstr = new GotoI(0); 

  //IF y != 0 GOTO ?
  parent_.codestream_ << ifinstr;
  //x = 1
  parent_.codestream_ << (new SimpleAssignmentI(new TempRegister(temp),makeOne()));
  //GOTO ?
  parent_.codestream_ << gotoinstr;
  //x = 0
  Label falseBranch = parent_.codestream_.nextInstructionLabel();
  parent_.codestream_ << (new SimpleAssignmentI(new TempRegister(temp),makeZero()));

  Label end = parent_.codestream_.nextInstructionLabel();
  //backpatch if
  ifinstr->setDestination(falseBranch);
  //backpatch goto
  gotoinstr->setDestination(end);
  result_ = new TempRegister(temp);
}

//generate t1 = - t2
void ICOpTranslator::visitUMinusOp(UMinusOp* op) {
  //declare temporary variable to hold the result
  //(resultType_ contains the right return type, arg1_ the argument)
  Variable* temp = parent_.scope_->declareTempVariable(resultType_);
  parent_.codestream_ << new UnAssignmentI(op,new TempRegister(temp),arg1_);
  result_ = new TempRegister(temp);
}

//generate t1 = t2 op t3
void ICOpTranslator::generateArithMeticOp(Operator* op) {
  //expects the arguments to be in arg1_ and arg2_ and the right
  //returntype to be in resultType_
  Variable* temp = parent_.scope_->declareTempVariable(resultType_);
  parent_.codestream_ << new BinAssignmentI(op,new TempRegister(temp),arg1_,arg2_);
  result_ = new TempRegister(temp);
}

/*
 * x < y should be translated to (< stands for any relop):
 *
 *        IF x < y GOTO true
 * false: result = 0
 *        GOTO end
 *  true: result = 1
 *   end: ...
 */

void ICOpTranslator::generateBooleanOp(Operator* op) {
  //declare temporary variable to hold the result
  Variable* temp = parent_.scope_->declareTempVariable(resultType_);
  IfI* ifinstr = generateTest(op,arg1_,arg2_);
  GotoI* gotoinstr = new GotoI(0); 

  //IF x < y GOTO ?
  parent_.codestream_ << ifinstr;
  //result = 0
  parent_.codestream_ << (new SimpleAssignmentI(new TempRegister(temp),makeZero()));
  //GOTO ?
  parent_.codestream_ << gotoinstr;
  //result = 1
  Label trueBranch = parent_.codestream_.nextInstructionLabel();
  parent_.codestream_ << (new SimpleAssignmentI(new TempRegister(temp),makeOne()));

  Label end = parent_.codestream_.nextInstructionLabel();
  //backpatch if
  ifinstr->setDestination(trueBranch);
  //backpatch goto
  gotoinstr->setDestination(end);
  result_ = new TempRegister(temp);
}

IfI* ICOpTranslator::generateTest(Operator* op,VarRegister* arg1, VarRegister* arg2) {
  return (new IfI(op,arg1,arg2,0));
}

NumRegister* ICOpTranslator::makeZero() {
  return new NumRegister(zero_);
}

NumRegister* ICOpTranslator::makeOne() {
  return new NumRegister(one_);
}
