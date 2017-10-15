#include "bexptranslator.h"

//Abstract Syntax Tree classes
#include "../syntaxtree/aggregates.h"
#include "../syntaxtree/block.h"
#include "../syntaxtree/ifstmt.h"
#include "../syntaxtree/whilestmt.h"
#include "../syntaxtree/assign.h"
#include "../syntaxtree/returnstmt.h"
#include "../syntaxtree/readstmt.h"
#include "../syntaxtree/writestmt.h"
#include "../syntaxtree/unop.h"
#include "../syntaxtree/binop.h"
#include "../syntaxtree/lengthexp.h"
#include "../syntaxtree/funcall.h"
#include "../syntaxtree/arrayaccess.h"
#include "../syntaxtree/number.h"
#include "../syntaxtree/qchar.h"

//Instruction classes
#include "../instructions/addressi.h"
#include "../instructions/arralloci.h"
#include "../instructions/arraccessi.h"
#include "../instructions/arrassigni.h"
#include "../instructions/binassigni.h"
#include "../instructions/calli.h"
#include "../instructions/gotoi.h"
#include "../instructions/ifi.h"
#include "../instructions/lengthi.h"
#include "../instructions/parami.h"
#include "../instructions/readi.h"
#include "../instructions/returni.h"
#include "../instructions/retvali.h"
#include "../instructions/simpleassigni.h"
#include "../instructions/unassigni.h"
#include "../instructions/writei.h"

#include "../instructions/varregister.h"
#include "../instructions/funregister.h"
#include "../instructions/arrayregister.h"
#include "../instructions/numregister.h"
#include "../instructions/charregister.h"
#include "../instructions/tempregister.h"

#include "../instructions/reglist.h"
#include "../operators/operators.h"

#include <stdexcept>
#include <iostream>


BexpTranslator::~BexpTranslator() {
  delete trueSet_;
  delete falseSet_;
  delete zero_;
}

/**
 * var
 *  translates to
 * code testing whether the value is zero or not
 */
void BexpTranslator::visitVariable(Variable* var) {
  //IF var != 0 GOTO ?true?
  nextInstrTrue();
  generateZeroTest(new VarRegister(var));

  //GOTO ?false?
  nextInstrFalse();
  generateZeroJump();
}

/**
 * f(exp1,exp2,exp3)
 *  translates to
 * code testing whether the returnvalue
 * of the function is zero or not
 */
void BexpTranslator::visitFuncall(Funcall* funcall) {
  //first, generate code for the function call itself
  funcall->accept(parent_);
  //the value holding the result is now stored in parent_->place_
  
  //IF returnvalue != 0 GOTO ?true?
  nextInstrTrue();
  generateZeroTest(parent_->place_);

  //GOTO ?false?
  nextInstrFalse();
  generateZeroJump();
}

/**
 * op exp
 *  translates to
 * code testing whether (op exp) is true or not
 * (if the operator is NOT, this method will just
 * switch the true and false sets)
 */
void BexpTranslator::visitUnaryOp(UnaryOp* unop) {

  //if the operation is NOT, just switch the true and false sets
  if (&unop->unaryOp() == &operatorNegate) {

    //let the expression be visited by myself (not by my parent)
    unop->expression()->accept(this);
    
    LabelSet* temp(falseSet_);
    falseSet_ = trueSet_;
    trueSet_ = temp;

  } else {
    //first, generate code for the unary operation itself 
    unop->accept(parent_);
    //the value holding the result is now stored in parent_->place_
  
    //IF (op exp) != 0 GOTO ?true?
    nextInstrTrue();
    generateZeroTest(parent_->place_);

    //GOTO ?false?
    nextInstrFalse();
    generateZeroJump();
  }
}

/**
 * exp1 op exp2
 *  translates to
 * code testing whether the result is zero or not
 * When dealing with relational operators however,
 * it is more efficient to generate a 'one-shot' branch instruction
 * like IF x < y THEN ... instead of first evaluating x < y by value
 * and then branching according to that result
 */
void BexpTranslator::visitBinaryOp(BinaryOp* binop) {
  currentBinop_ = binop;
  //make a decision according to the type of the operator
  binop->binaryOp().accept(this);
}

void BexpTranslator::generateArithmeticBinop() {
  //first, generate code for the binary arithmetic operation itself 
  currentBinop_->accept(parent_);
  //the value holding the result is now stored in parent_->place_
  
  //IF (exp1 op exp2) != 0 GOTO ?true?
  nextInstrTrue();
  generateZeroTest(parent_->place_);

  //GOTO ?false?
  nextInstrFalse();
  generateZeroJump();
}

void BexpTranslator::generateBooleanBinop() {
  Operator& op = currentBinop_->binaryOp();
  //evaluate both arguments
  currentBinop_->firstArg()->accept(parent_);
  VarRegister* arg1 = parent_->place_;
  currentBinop_->secondArg()->accept(parent_);
  VarRegister* arg2 = parent_->place_;

  nextInstrTrue();
  //IF binop.arg1 op binop.arg2 THEN ?true?
  parent_->codestream_ << (new IfI(&op,arg1,arg2,0));

  //GOTO ?false?
  nextInstrFalse();
  generateZeroJump();
}

/**
 * num
 *  translates to
 * code testing whether num is zero or not
 */
void BexpTranslator::visitNumber(Number* num) {
  //IF num != 0 GOTO ?true?
  nextInstrTrue();
  generateZeroTest(new NumRegister(num));

  //GOTO ?false?
  nextInstrFalse();
  generateZeroJump();
}

/**
 * char
 *  translates to
 * code testing whether char is zero or not
 */
void BexpTranslator::visitQChar(QChar* chr) {
  //IF chr != 0 GOTO ?true?
  nextInstrTrue();
  generateZeroTest(new QCharRegister(chr));

  //GOTO ?false?
  nextInstrFalse();
  generateZeroJump();
}

/**
 * length exp
 *  translates to
 * code testing whether the result is zero or not
 */
void BexpTranslator::visitLengthExpression(LengthExpression* len) {
  //first, generate code for the length expression itself 
  len->accept(parent_);
  //the value holding the result is now stored in parent_->place_
  
  //IF (length exp) != 0 GOTO ?true?
  nextInstrTrue();
  generateZeroTest(parent_->place_);

  //GOTO ?false?
  nextInstrFalse();
  generateZeroJump();
}

/**
 * a[exp1,...,expn]
 *  translates to
 * code testing whether the result is zero or not
 */
void BexpTranslator::visitArrayAccess(ArrayAccess* arrayaccess) {
  //first, generate code for the array access itself 
  arrayaccess->accept(parent_);
  //the value holding the result is now stored in parent_->place_
  
  //IF A[I] != 0 GOTO ?true?
  nextInstrTrue();
  generateZeroTest(parent_->place_);

  //GOTO ?false?
  nextInstrFalse();
  generateZeroJump();
}

void BexpTranslator::visitDividesOp(DividesOp* op) {
  generateArithmeticBinop();
}

void BexpTranslator::visitEqualOp(EqualOp* op) {
  generateBooleanBinop();
}

void BexpTranslator::visitGreaterOp(GreaterOp* op) {
  generateBooleanBinop();
}

void BexpTranslator::visitLessOp(LessOp* op) {
  generateBooleanBinop();
}

void BexpTranslator::visitMinusOp(MinusOp* op) {
  generateArithmeticBinop();
}

void BexpTranslator::visitNEqualOp(NEqualOp* op) {
  generateBooleanBinop();
}

void BexpTranslator::visitPlusOp(PlusOp* op) {
  generateArithmeticBinop();
}


void BexpTranslator::visitTimesOp(TimesOp* op) {
  generateArithmeticBinop();
}

void BexpTranslator::nextInstrTrue() {
  trueSet_->insert(parent_->codestream_.nextInstructionLabel());
}


void BexpTranslator::nextInstrFalse() {
  falseSet_->insert(parent_->codestream_.nextInstructionLabel());
}

NumRegister* BexpTranslator::generateZero() const {
  return new NumRegister(zero_);
}

void BexpTranslator::generateZeroTest(VarRegister* var) {
  parent_->codestream_ << (new IfI(&operatorNEqual,var,generateZero(),0));
}

void BexpTranslator::generateZeroJump() {
  parent_->codestream_ << (new GotoI(0));
}
