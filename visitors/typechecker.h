#ifndef VIS_TYPECHECKER_H
#define VIS_TYPECHECKER_H

#include "treevisitor.h"
#include "../type.h"
#include "typecheckexc.h"

using namespace std;

typedef list<TypeCheckError*> TypeCheckErrors;
typedef TypeCheckErrors::iterator TypeCheckErrit;

/**
 * A TypeCheckVisitor descends an abstract syntax tree and checks, for every
 * node, the validity of the statement or expression
 */

class TypeCheckVisitor: public TreeVisitor {

   public:

      TypeCheckVisitor(): errors_(new TypeCheckErrors) { }

      virtual ~TypeCheckVisitor();

      /**
       * @return all errors which occured during typechecking
       */
      TypeCheckErrors& errors() const { return *errors_; }

      /**
       * @return the number of typecheck errors
       */
      int numerrors() const { return errors_->size(); }

      /**
       * To typecheck a block, typecheck all its statements
       */
      virtual void visitBlock(Block* block);

      /**
       * To typecheck an if statement, check its condition
       * and its branches
       * Also make sure condition evaluates to integral type
       */
      virtual void visitIfStatement(IfStatement* ifstmt);

      /**
       * To typecheck a while statement, typecheck its condition
       * and its body
       * Also make sure condition evaluates to integral type
       */
      virtual void visitWhileStatement(WhileStatement* whilestmt);

      /**
       * To typecheck an assignment, make sure the type of the
       * assigning expression conforms to the type of the target
       */
      virtual void visitAssignment(Assignment* assgn);

      /**
       * To typecheck a return statement, make sure the returned
       * expression's type conforms to the current function's
       * returntype
       */
      virtual void visitReturnStatement(ReturnStatement* ret);

      /**
       * To typecheck a function call, make sure the arguments
       * match both in number and in type
       */
      virtual void visitFuncall(Funcall* funcall);

      /**
       * To typecheck a read statement, just make sure you don't
       * read into a variable denoting a function
       */
      virtual void visitReadStatement(ReadStatement* readstmt);

      /**
       * To typecheck a write statement, just make sure you don't
       * write from a variable denoting a function
       */
      virtual void visitWriteStatement(WriteStatement* writestmt);

      /**
       * To typecheck a unary operator, make sure its operator can
       * handle the expression's type
       */
      virtual void visitUnaryOp(UnaryOp* unop);

      /**
       * To typecheck a binary operator, make sure its operator can
       * handle its argument types
       */
      virtual void visitBinaryOp(BinaryOp* binop);

      /**
       * To typecheck a length expression, check whether we're asking
       * the length of an array
       */
      virtual void visitLengthExpression(LengthExpression* len); 

      /**
       * To typecheck an array reference, make sure the indices are
       * integers and typecheck the index expressions
       */
      virtual void visitArrayAccess(ArrayAccess* arrayaccess); 

      /**
       * To typecheck an array, check all its index expressions
       * and make sure they evaluate to integral types
       */
      virtual void visitArray(Array* array);

      /**
       * To typecheck a function, typecheck it's body
       */
      virtual void visitFunction(Function* fun);

   private:

      Function* function_;
      TypeCheckErrors* errors_;

};

#endif
