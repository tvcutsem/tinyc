#include "livenesschecker.h"

//Abstract Syntax Tree files
#include "../syntaxtree/readstmt.h"
#include "../syntaxtree/assign.h"
#include "../syntaxtree/arrayaccess.h"
#include "../syntaxtree/writestmt.h"
#include "../syntaxtree/returnstmt.h"
#include "../syntaxtree/emptystmt.h"
#include "../syntaxtree/ifstmt.h"
#include "../syntaxtree/whilestmt.h"
#include "../syntaxtree/block.h"
#include "../syntaxtree/binop.h"
#include "../syntaxtree/unop.h"
#include "../syntaxtree/function.h"
#include "../syntaxtree/funcall.h"
#include "../syntaxtree/variable.h"
#include "../syntaxtree/assign.h"
#include "../syntaxtree/number.h"
#include "../syntaxtree/qchar.h"
#include "../syntaxtree/lengthexp.h"
#include "../syntaxtree/vardecl.h"

#include "../type.h"

#include <stdexcept>

LivenessChecker::LivenessChecker():
    uninitialized_(new VariableSet),
    unused_(new VariableSet),
    returnVar_(new VariableDec(StringPool::instance()->getString("RETURN"),
	       void_t,false)),
    status(CHECKING_VAR) { }

LivenessChecker::~LivenessChecker() { 
  delete uninitialized_;
  delete unused_;
  delete returnVar_;
}

void LivenessChecker::check(Identifier* id, ControlErrors& errors) {
  errors_ = &errors;
  if (id->isFunction())
    id->accept(this);
}

void LivenessChecker::visitVariable(Variable* var) {

  //variable is being used
  unused_->remove(var);

  switch(status) {
    //when initializing this variable, remove it from the uninitialized set
    case INITIALIZING_VAR:
      uninitialized_->remove(var);
      break;
    //when checking this variable, see to it that it is not used before it
    //is properly initialized
    case CHECKING_VAR:
      if (uninitialized_->contains(var)) {
	errors_->push_back(UninitializedVariableError(*var));
      }
      break;
    default:
      throw std::logic_error("Fallen off the switch in LivenessChecker::visitVariable");
  }
}

void LivenessChecker::visitFunction(Function* fun) {
  //add the dummy RETURN variable to the set if the function's return type is non-void
  if (fun->returnType() != void_t)
    uninitialized_->insert(returnVar_);

  //visit the function body
  fun->body()->accept(this);

  //if RETURN is still in the set, report error
  if (uninitialized_->contains(returnVar_)) {
    errors_->push_back(MissingReturnError(*fun));
    //remove it because otherwise it will be counted as a non-used variable
    uninitialized_->erase(returnVar_);
  }

  //report errors for each unused variable
  typedef VariableSet::iterator viterator;
  for (viterator v = unused_->begin(); v != unused_->end(); v++) {
    // *v is a Variable*
    errors_->push_back(UnusedVariableError(**v));
  }

  //clear the sets for the next function to check
  uninitialized_->clear();
  unused_->clear();
}

void LivenessChecker::visitBlock(Block* b) {
  //add all variable declarations to the uninitialized set
  SymbolTable* declarations(b->scope());
  Variables declaredVars;
  declarations->retrieveVariables(declaredVars);

  uninitialized_->add(declaredVars);
  unused_->add(declaredVars);

  Statements* body(b->body());
  for (Statementit s = body->begin(); s != body->end(); s++)
    // *s is a Statement*
    (*s)->accept(this);
}

void LivenessChecker::visitIfStatement(IfStatement* ifstmt) {
  //first of all, visit the if condition with the current uninitialized set
  ifstmt->condition()->accept(this);

  //next, create a copy of the current set for both branches
  VariableSet firstBranchSet(*uninitialized_);
  VariableSet secondBranchSet(*uninitialized_);
  //keep a reference to the old set
  VariableSet* save(uninitialized_);

  //visit the true branch
  uninitialized_ = &firstBranchSet;
  ifstmt->truebranch()->accept(this);

  //visit the second branch
  uninitialized_ = &secondBranchSet;
  ifstmt->falsebranch()->accept(this);

  //set uninitalized_ to the union of both sets
  save->clear();
  // uninitialized_ = firstBranchSet U secondBranchSet
  setUnion(*save,firstBranchSet,secondBranchSet);

  //restore the old set
  uninitialized_ = save;
}

void LivenessChecker::visitWhileStatement(WhileStatement* whilestmt) {
  //first, visit the while condition with the current set
  whilestmt->condition()->accept(this);
  
  //make a copy of the original set and visit the while body with it
  VariableSet copy(*uninitialized_);
  //keep a reference to the old set
  VariableSet* save(uninitialized_);
  uninitialized_ = &copy;

  whilestmt->body()->accept(this);

  //restore the original set, all declarations from the while body are lost
  uninitialized_ = save;
}

void LivenessChecker::visitAssignment(Assignment* assgn) {
  //first, check the assignment's expression using the current set
  //ie if the statement is
  //  x = x + y
  //where x has not yet been initialized before, this is also an error,
  //so it is imperative to visit x + y first

  assgn->expression()->accept(this);

  //visit the left-hand side, we are now in INITIALIZING status
  //don't go to INITIALIZING status is the left-hand side is an
  //array access, ie when visiting a[x,y,z] = ... we are clearly
  //NOT initializing x, y and z.
  if (!assgn->target()->isArrayAccess())
    status = INITIALIZING_VAR;
  assgn->target()->accept(this);
  status = CHECKING_VAR;
}

void LivenessChecker::visitReturnStatement(ReturnStatement* ret) {
  //propagate
  ret->expression()->accept(this);

  //remove the dummy RETURN variable from the set
  uninitialized_->remove(returnVar_);
}

void LivenessChecker::visitFuncall(Funcall* funcall) {
  //visit the actual parameters
  Expressions* actuals(funcall->arguments());
  for (Expressionit e = actuals->begin(); e != actuals->end(); e++)
    // *e is an Expression*
    (*e)->accept(this);
}

void LivenessChecker::visitReadStatement(ReadStatement* readstmt) {
  //mark the target
  //don't go to INITIALIZING status is the read is an
  //array access, ie when visiting read a[x,y,z];
  //we are clearly not initializing x, y and z then.
  if (!readstmt->target()->isArrayAccess())
    status = INITIALIZING_VAR;
  readstmt->target()->accept(this);
  status = CHECKING_VAR;
}

void LivenessChecker::visitWriteStatement(WriteStatement* writestmt) {
  //propagate
  writestmt->expression()->accept(this);
}

void LivenessChecker::visitUnaryOp(UnaryOp* unop) {
  //propagate
  unop->expression()->accept(this);
}

void LivenessChecker::visitBinaryOp(BinaryOp* binop) {
  //propagate
  binop->firstArg()->accept(this);
  binop->secondArg()->accept(this);
}

void LivenessChecker::visitLengthExpression(LengthExpression* len) {
  //propagate
  len->lexpression()->accept(this);
}

void LivenessChecker::visitArrayAccess(ArrayAccess* arrayaccess) {
  //propagate
  Expressions& indices(arrayaccess->indices());
  for (Expressionit idx = indices.begin(); idx != indices.end(); idx++)
    // *idx is an Expression*
    (*idx)->accept(this);
}
