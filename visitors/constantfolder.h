#ifndef CONSTANT_FOLDER_H
#define CONSTANT_FOLDER_H

#include "../syntaxtree/aggregates.h"
#include "../syntaxtree/node.h"
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
#include "../syntaxtree/constant.h"
#include "../syntaxtree/break.h"
#include "../syntaxtree/continue.h"

using namespace std;

/**
 * A ConstantFolder traverses the abstract syntax tree and
 * tries to fold operations on constants, eg it will try to
 * replace:
 *     binop(+)
 *     /   \
 *  num(3) num(2)
 *
 * with:
 *     num(5)
 *
 * Furthermore, while and if's can be folded into simple
 * statements if we can determine the outcome of their
 * boolean test at compile-time, ie.
 *   while(0) { ... } can be safely skipped,
 *   if(0){then} else {else} can be converted to {else}
 */

class ConstantFolder: public TreeVisitor {

   public:

      virtual ~ConstantFolder() { }

      virtual void visitVariable(Variable* var);

      virtual void visitFunction(Function* fun);

      virtual void visitArray(Array* array);
      
      virtual void visitBlock(Block* b);

      virtual void visitIfStatement(IfStatement* ifstmt);

      virtual void visitWhileStatement(WhileStatement* whilestmt);

      virtual void visitAssignment(Assignment* assgn);

      virtual void visitReturnStatement(ReturnStatement* ret);

      virtual void visitFuncall(Funcall* funcall);

      virtual void visitReadStatement(ReadStatement* readstmt);

      virtual void visitWriteStatement(WriteStatement* writestmt);

      virtual void visitUnaryOp(UnaryOp* unop);

      virtual void visitBinaryOp(BinaryOp* binop);

      virtual void visitNumber(Number* num);

      virtual void visitQChar(QChar* chr);

      virtual void visitLengthExpression(LengthExpression* len);

      virtual void visitArrayAccess(ArrayAccess* arrayaccess);
      
      virtual void visitEmptyStatement(EmptyStatement* empty);

      virtual void visitBreakStatement(BreakStatement* breakstmt);

      virtual void visitContinueStatement(ContinueStatement* ctustmt);

   private:

      //these functions are used to downcast the 'global' result variable
      //into an expression, lexpression or statement
      Expression*  getExp();
      LExpression* getLExp();
      Statement*   getStmt();
      Block*       getBlock();

      /**
       * Keeps the result of a previous traversal, acts as the returnvalue
       * of a visit method
       */
      Node* result_;

};

#endif
