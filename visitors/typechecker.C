#include "typechecker.h"

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

#include <iostream>

//destructor
TypeCheckVisitor::~TypeCheckVisitor() {
   for (TypeCheckErrors::iterator e=errors_->begin();e!=errors_->end();e++){
      //*e is a TypeCheckError*
      delete *e;
   }
   delete errors_;
}

/**
 * To typecheck a block, typecheck all its statements
 */
void TypeCheckVisitor::visitBlock(Block* block) {
   Statements* body = block->body();
   Identifiers ids;
   //fill ids with the local block declarations
   block->scope()->declarations(ids);
   for (Identifierit local=ids.begin();local!=ids.end(); local++) {
     (*local)->accept(this);
   }
   for (Statementit s=body->begin(); s!=body->end();s++) {
      //*s is a Statement*
      (*s)->accept(this);
   }
}

/**
 * To typecheck an if statement, check its condition
 * and its branches
 * Make sure condition is of integral type
 */
void TypeCheckVisitor::visitIfStatement(IfStatement* ifstmt) {
   ifstmt->condition()->accept(this);
   ifstmt->truebranch()->accept(this);
   ifstmt->falsebranch()->accept(this);

   Expression* condition = ifstmt->condition();
   if (!coerces_to(condition->type(),int_t)) {
      errors_->push_back(new ConditionError(condition->type(),condition));
   }
}

/**
 * To typecheck a while statement, typecheck its condition
 * and its body
 * Make sure condition is of integral type
 */
void TypeCheckVisitor::visitWhileStatement(WhileStatement* whilestmt) {
   whilestmt->condition()->accept(this);
   whilestmt->body()->accept(this);

   Expression* condition = whilestmt->condition();
   if (!coerces_to(condition->type(),int_t)) {
      errors_->push_back(new ConditionError(condition->type(),condition));
   }
}

/**
 * To typecheck an assignment, make sure the type of the
 * assigning expression conforms to the type of the target
 */
void TypeCheckVisitor::visitAssignment(Assignment* assgn) {
   assgn->expression()->accept(this);
   assgn->target()->accept(this);
   Type exprType = assgn->expression()->type();
   Type varType = assgn->target()->type();

   if (varType == array_t) {
      errors_->push_back(new UnexpectedArrayError(assgn)); 
      return;
   }
   if (varType == fun_t) {
      errors_->push_back(new UnexpectedFunctionError(assgn));
      return;
   }
   if (!coerces_to(exprType,varType)) {
      errors_->push_back(new AssignmentError(varType,exprType,assgn));
   }
}

/**
 * To typecheck a return statement, make sure the returned
 * expression's type conforms to the current function's
 * returntype
 */
void TypeCheckVisitor::visitReturnStatement(ReturnStatement* ret) {
   ret->expression()->accept(this);
   Type typeToReturn = ret->expression()->type();
   Type typeExpected = function_->returnType();
   if (!coerces_to(typeToReturn,typeExpected)) {
      errors_->push_back(new ReturnError(typeExpected,typeToReturn,ret));
   }
}

/**
 * To typecheck a function call, make sure the arguments
 * match both in number and in type
 */
void TypeCheckVisitor::visitFuncall(Funcall* funcall) {
   Identifiers formals;
   funcall->callee()->parameters(formals);
   Expressions* actuals = funcall->arguments();
   if (formals.size() != actuals->size()) {
      errors_->push_back(new ArityError(formals.size(),actuals->size(),funcall));
      return;
   }

   //don't get here if size doesn't match

   Expressionit actual(actuals->begin());
   Identifierit formal(formals.begin());
   int argcnt(1);
   while (actual != actuals->end()) {
      //*actual is an Expression*
      //*formal is an Identifier*

      //typecheck actual parameter
      (*actual)->accept(this);

      //check whether types match
      if (!coerces_to((*actual)->type(),(*formal)->type())) {
	 errors_->push_back(new ParameterTypeError(
	       argcnt,
	       (*formal)->type(),
	       (*actual)->type(),
	       funcall));
         //don't typecheck these arguments any further
         actual++; formal++; argcnt++; continue; 
      }

      //if we are dealing with arrays, the base types should match and
      //the dimensionality should be the same
      if ((*actual)->type() == array_t) {
	Array* actualArray = dynamic_cast<Array*>(*actual);
	Array* formalArray = dynamic_cast<Array*>(*formal);
	if (!coerces_to(actualArray->baseType(),formalArray->baseType())) {
	  errors_->push_back(new ArrayBaseMismatch(
		argcnt,
		formalArray->baseType(),
		actualArray->baseType(),
		funcall));
	}
	if (actualArray->dimension() != formalArray->dimension()) {
	  errors_->push_back(new DimensionMismatch(
		argcnt,
		formalArray->dimension(),
		actualArray->dimension(),
		funcall));
	}
      }

      actual++;
      formal++;
      argcnt++;
   }
}

/**
 * To typecheck a read statement, just make sure you don't
 * read into a variable denoting a function
 */
void TypeCheckVisitor::visitReadStatement(ReadStatement* readstmt) {
   readstmt->target()->accept(this);
   if (readstmt->target()->type() == array_t) {
      errors_->push_back(new UnexpectedArrayError(readstmt)); 
      return;
   }
   if (readstmt->target()->type() == fun_t) {
      errors_->push_back(new UnexpectedFunctionError(readstmt));
   }
}

/**
 * To typecheck a write statement, just make sure you don't
 * write from a variable denoting a function
 */
void TypeCheckVisitor::visitWriteStatement(WriteStatement* writestmt) {
   writestmt->expression()->accept(this);
   if (writestmt->expression()->type() == array_t) {
      errors_->push_back(new UnexpectedArrayError(writestmt)); 
      return;
   }
   if (writestmt->expression()->type() == fun_t) {
      errors_->push_back(new UnexpectedFunctionError(writestmt));
   }
}

/**
 * To typecheck a unary operator, make sure its operator can
 * handle the expression's type
 */
void TypeCheckVisitor::visitUnaryOp(UnaryOp* unop) {
   unop->expression()->accept(this);
   Expression* expr = unop->expression();
   if (!unop->unaryOp().canHandle(expr->type())) {
      errors_->push_back(new OperatorTypeError(expr->type(),unop));
   }
}

/**
 * To typecheck a binary operator, make sure its operator can
 * handle its argument types
 */
void TypeCheckVisitor::visitBinaryOp(BinaryOp* binop) {
   Expression* first = binop->firstArg();
   Expression* second = binop->secondArg();
   first->accept(this);
   second->accept(this);

   Operator& op(binop->binaryOp());
   if (!op.canHandle(first->type())) {
      errors_->push_back(new OperatorTypeError(first->type(),binop));
   }
   if (!op.canHandle(second->type())) {
      errors_->push_back(new OperatorTypeError(second->type(),binop));
   }
}

/**
 * To typecheck a length expression, check whether we're asking
 * the length of an array
 */
void TypeCheckVisitor::visitLengthExpression(LengthExpression* len) {
   len->lexpression()->accept(this);
   if (len->lexpression()->type() != array_t) {
      errors_->push_back(new ArrayExpectedError(len));
   }
}

/**
 * To typecheck an array reference, make sure all indices are integers
 * and typecheck the index expressions. Also check whether the dimensions
 * match
 */
void TypeCheckVisitor::visitArrayAccess(ArrayAccess* arrayaccess) {
   Expressions& actualindices = arrayaccess->indices();
   Expressions& formalindices = arrayaccess->array()->lengths();
   if (actualindices.size() != formalindices.size()) {
      errors_->push_back(new ArrayDimensionError(
	       formalindices.size(),
	       actualindices.size(),
	       arrayaccess));
      return;
   }

   Expressionit actual(actualindices.begin());
   int argcnt(1);
   while (actual != actualindices.end()) {
      //*actual is an Expression*
      if (!coerces_to((*actual)->type(),int_t)) {
	 errors_->push_back(new IllegalIndexError(argcnt,(*actual)->type(),arrayaccess));
      }

      actual++;
      argcnt++;
   }
}

/**
 * To typecheck an array, make sure all indices evaluate to integral values
 */
void TypeCheckVisitor::visitArray(Array* array) {
   Expressions& indices = array->lengths();
   int argcnt(1);
   for(Expressionit index(indices.begin());index!=indices.end();index++) {
      //*index is an Expression*
      (*index)->accept(this);
      if (!coerces_to((*index)->type(),int_t)) {
	 errors_->push_back(new IllegalIndexError(argcnt,(*index)->type(),array));
      }
      argcnt++;
   }
}

/**
 * To typecheck a function, typecheck it's body
 */
void TypeCheckVisitor::visitFunction(Function* fun) {
   //do nothing if this function is the same as the current one
   //to avoid infinite recursion
   if (*fun == *function_) {
      return;
   }
   //set current function (for return type inspection)
   function_ = fun;
   fun->body()->accept(this);
}

