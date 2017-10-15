#include "ifstmt.h"

#include <sstream>

IfStatement::IfStatement(Expression* cond,
                         Statement* thenbranch, Statement* elsebranch):
  cond_(cond),then_(thenbranch),else_(elsebranch),
  preserveTrue_(false),preserveFalse_(false) {
  //so that we don't have to explicitly test for empty else
  //branch each time
  if (elsebranch==0)
    else_ = new EmptyStatement;
}

IfStatement::~IfStatement() {
   delete cond_;
   if (!preserveTrue_)
     delete then_;
   if (!preserveFalse_)
     delete else_;
}

void IfStatement::setCondition(Expression* newCond) {
  if (cond_ == newCond)
    return;
  delete cond_;
  cond_ = newCond;
}

void IfStatement::setTrueBranch(Statement* newTrue) {
  if (then_ == newTrue)
    return;
  delete then_;
  then_ = newTrue;
}

void IfStatement::setFalseBranch(Statement* newFalse) {
  if (else_ == newFalse)
    return;
  delete else_;
  else_ = newFalse;
}

ostream& IfStatement::prettyPrint(ostream& os,int indent) const {
   while(indent--)
      os << " ";
   os << "if (";
   cond_->prettyPrint(os,indent);
   os << ")\n";
   then_->prettyPrint(os,indent+3);
   os << "\n";
   else_->prettyPrint(os,indent+3);
   return os;
}

string IfStatement::toString() const {
   stringstream ss;
   ss << "[ If [" << cond_->toString() << "]\n";
   ss << "[ " << then_->toString() << " ]\n";
   ss << "[ " << else_->toString() << " ]]";
   return ss.str();
}
