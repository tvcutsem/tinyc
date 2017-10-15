#include "constantfolder.h"
#include "../syntaxtree/emptystmt.h" 

#include <stdexcept>

void ConstantFolder::visitVariable(Variable* var) {
  //do nothing
  result_ = var;
}

void ConstantFolder::visitFunction(Function* fun) {
  //visit the function's body
  fun->body()->accept(this);
  //getStmt now returns the (hopefully) optimized function body
  fun->setBody(getBlock());
  result_ = fun;
}

void ConstantFolder::visitArray(Array* array) {
  //optimize the array indices
  Expressions& lengths = array->lengths();
  for (Expressionit i=lengths.begin();i!=lengths.end();i++) {
    //*i is an Expression*
    (*i)->accept(this);
    if ((*i) == getExp())
      continue;
    delete *i;
    *i = getExp();
  }
  result_ = array;
}

void ConstantFolder::visitBlock(Block* b) {
  //optimize eg array declarations in the local declarations of the block
  Identifiers ids;
  b->scope()->declarations(ids);
  for (Identifierit local=ids.begin();local!=ids.end(); local++) {
    (*local)->accept(this);
  }
  
  //optimize each statement
  Statements* stmts = b->body();
  for (Statementit s=stmts->begin();s!=stmts->end();s++) {
    //*s is a Statement*
    (*s)->accept(this);
    //if a statement has not changed, to not modify it
    if ((*s) == getStmt())
      continue;
    *s = getStmt();
  }
  result_ = b;
}

void ConstantFolder::visitIfStatement(IfStatement* ifstmt) {
  ifstmt->truebranch()->accept(this);
  //getStmt() now returns the folded statements of the truebranch
  ifstmt->setTrueBranch(getStmt());
  ifstmt->falsebranch()->accept(this);
  //getStmt() now returns the folded statements of the falsebranch
  ifstmt->setFalseBranch(getStmt());

  ifstmt->condition()->accept(this);
  //if our expression is a constant, we can evaluate the if
  //at compile-time
  if (getExp()->isConstant()) {
    Constant* c = dynamic_cast<Constant*>(getExp());
    if (c->isTrue()) {
      result_ = ifstmt->truebranch();
      //to avoid that if destructor will destroy the branch
      ifstmt->preserveTrueBranch();
    } else {
      result_ = ifstmt->falsebranch();
      //to avoid that if destructor will destroy the branch
      ifstmt->preserveFalseBranch();
    }
    //now get rid of the obsolete if
    delete ifstmt;
  } else { 
    //no constant, but perhaps a folded condition
    ifstmt->setCondition(getExp());
    result_ = ifstmt;
  }
}

void ConstantFolder::visitWhileStatement(WhileStatement* whilestmt) {
  whilestmt->body()->accept(this);
  //getStmt() now returns the folded statements of the while body
  whilestmt->setBody(getStmt());

  whilestmt->condition()->accept(this);
  //if our expression is a constant and evaluates to 0, skip the while
  if (getExp()->isConstant()) {
    Constant* c = dynamic_cast<Constant*>(getExp());
    if (c->isFalse()) {
      result_ = new EmptyStatement; 
      delete whilestmt;
      return;
    }
  }
  //if cond was not a constant or was true, just return folded while
  whilestmt->setCond(getExp());
  result_ = whilestmt;
}

void ConstantFolder::visitAssignment(Assignment* assgn) {
  //visit both parts of the assign, fold them, and return the assignment
  assgn->target()->accept(this);
  assgn->setTarget(getLExp());
  assgn->expression()->accept(this);
  assgn->setExpression(getExp());
  result_ = assgn;
}

void ConstantFolder::visitReturnStatement(ReturnStatement* ret) {
  ret->expression()->accept(this);
  ret->setExpression(getExp());
  result_ = ret;
}

void ConstantFolder::visitFuncall(Funcall* funcall) {
  //fold all actual argument expressions
  Expressions* args = funcall->arguments();
  for (Expressionit e=args->begin();e!=args->end();e++) {
    //*e is an Expression*
    (*e)->accept(this);
    if ((*e) == getExp())
      continue;
    delete *e;
    *e = getExp();
  }
  result_ = funcall;
}

void ConstantFolder::visitReadStatement(ReadStatement* readstmt) {
  readstmt->target()->accept(this);
  readstmt->setTarget(getLExp());
  result_ = readstmt;
}

void ConstantFolder::visitWriteStatement(WriteStatement* writestmt) {
  writestmt->expression()->accept(this);
  writestmt->setExpression(getExp());
  result_ = writestmt;
}

void ConstantFolder::visitUnaryOp(UnaryOp* unop) {
  //if the argument of the unary op is a constant, evaluate it
  unop->expression()->accept(this);
  unop->setExpression(getExp());

  if (getExp()->isConstant()) {
    Constant* c = dynamic_cast<Constant*>(getExp());
    //the second argument to apply is a dummy, this is a unary op
    Number dummy(0);
    result_ = unop->unaryOp().evaluate(c,&dummy);
  } else {
    result_ = unop;
  }
}

void ConstantFolder::visitBinaryOp(BinaryOp* binop) {
  //the reason why we do constant folding: trying to reduce the
  //number of nodes in the abstract syntax tree by evaluating at
  //compile-time
  Expression* exp1;
  Expression* exp2;
  binop->firstArg()->accept(this);
  exp1 = getExp();
  binop->setFirstArg(exp1);
  binop->secondArg()->accept(this);
  exp2 = getExp();
  binop->setSecondArg(exp2);

  //if both arguments are constants, evaluate them
  if (exp1->isConstant()&&exp2->isConstant()) {
    Constant* c1 = dynamic_cast<Constant*>(exp1);
    Constant* c2 = dynamic_cast<Constant*>(exp2);
    result_ = binop->binaryOp().evaluate(c1,c2);
  } else {
    result_ = binop;
  }
}

void ConstantFolder::visitNumber(Number* num) {
  result_ = num;
}

void ConstantFolder::visitQChar(QChar* chr) {
  result_ = chr;
}

void ConstantFolder::visitLengthExpression(LengthExpression* len) {
  len->lexpression()->accept(this);
  len->setLExpression(getLExp());
  result_ = len;
}

void ConstantFolder::visitArrayAccess(ArrayAccess* arrayaccess) {
  //fold all indices
  Expressions& indices = arrayaccess->indices();
  for (Expressionit i=indices.begin();i!=indices.end();i++) {
    //*i is an Expression*
    (*i)->accept(this);
    if ((*i) == getExp())
      continue;
    delete *i;
    *i = getExp();
  }
  result_ = arrayaccess;
}

void ConstantFolder::visitEmptyStatement(EmptyStatement* empty) {
  result_ = empty;
}

void ConstantFolder::visitBreakStatement(BreakStatement* breakstmt) {
  result_ = breakstmt;
}

void ConstantFolder::visitContinueStatement(ContinueStatement* ctustmt) {
  result_ = ctustmt;
}

Expression* ConstantFolder::getExp() {
  Expression* e = dynamic_cast<Expression*>(result_);
  if (e)
    return e;
  else
    throw logic_error("ConstantFolder: getExp not called on an expression: "
	              + result_->toString());
}

LExpression* ConstantFolder::getLExp() {
  LExpression* e = dynamic_cast<LExpression*>(result_);
  if (e)
    return e;
  else
    throw logic_error("ConstantFolder: getLExp not called on an lexpression: "
	              + result_->toString());
}

Statement* ConstantFolder::getStmt() {
  Statement* s = dynamic_cast<Statement*>(result_);
  if (s)
    return s;
  else
    throw logic_error("ConstantFolder: getStmt not called on a statement: "
	              + result_->toString());
}

Block* ConstantFolder::getBlock() {
  Block* s = dynamic_cast<Block*>(result_);
  if (s)
    return s;
  else
    throw logic_error("ConstantFolder: getBlock not called on a block: "
	              + result_->toString());
}
