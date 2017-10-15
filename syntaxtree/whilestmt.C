#include "whilestmt.h"

#include <sstream>

WhileStatement::WhileStatement(Expression* cond,Statement* body):
  cond_(cond),body_(body) { }

void WhileStatement::setCond(Expression* newCond) {
  if (cond_ == newCond)
    return;
  delete cond_;
  cond_ = newCond;
}

void WhileStatement::setBody(Statement* newBody) {
  if (body_ == newBody)
    return;
  delete body_;
  body_ = newBody;
}

ostream& WhileStatement::prettyPrint(ostream& os,int indent) const {
   while(indent--)
      os << " ";
   os << "while (";
   cond_->prettyPrint(os);
   os << ")\n";
   body_->prettyPrint(os,indent+3);
   return os;
}

string WhileStatement::toString() const {
   stringstream ss;
   ss << "[ While [" << cond_->toString() << "]";
   ss << "[" << body_->toString() << "]]";
   return ss.str();
}
