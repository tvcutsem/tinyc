#ifndef TREE_VISITOR_H
#define TREE_VISITOR_H

using namespace std;

//forward declarations

class Variable;
class Function;
class Array;
class Block;
class IfStatement;
class WhileStatement;
class Assignment;
class ReturnStatement;
class Funcall;
class ReadStatement;
class WriteStatement;
class UnaryOp;
class BinaryOp;
class Number;
class QChar;
class LengthExpression;
class ArrayAccess;
class EmptyStatement;
class UndefinedIdentifier;
class BreakStatement;
class ContinueStatement;

/**
 * Abstract superclass of all visitors.
 * For convenience, TreeVisitor gives all methods a null implementation,
 * so that its subclasses only have to override the methods they want
 */

class TreeVisitor {

   public:

      /** virtual destructor, this class will be subclassed */
      virtual ~TreeVisitor() { }

      virtual void visitVariable(Variable* var) { }

      virtual void visitFunction(Function* fun) { }

      virtual void visitArray(Array* array) { }
      
      virtual void visitBlock(Block* b) { }

      virtual void visitIfStatement(IfStatement* ifstmt) { }

      virtual void visitWhileStatement(WhileStatement* whilestmt) { }

      virtual void visitAssignment(Assignment* assgn) { }

      virtual void visitReturnStatement(ReturnStatement* ret) { }

      virtual void visitFuncall(Funcall* funcall) { }

      virtual void visitReadStatement(ReadStatement* readstmt) { }

      virtual void visitWriteStatement(WriteStatement* writestmt) { }

      virtual void visitUnaryOp(UnaryOp* unop) { }

      virtual void visitBinaryOp(BinaryOp* binop) { }

      virtual void visitNumber(Number* num) { }

      virtual void visitQChar(QChar* chr) { }

      virtual void visitLengthExpression(LengthExpression* len) { }

      virtual void visitArrayAccess(ArrayAccess* arrayaccess) { }
      
      virtual void visitEmptyStatement(EmptyStatement* empty) { }

      virtual void visitUndefinedIdentifier(UndefinedIdentifier* i) { }

      virtual void visitBreakStatement(BreakStatement* breakstmt) { }

      virtual void visitContinueStatement(ContinueStatement* ctustmt) { }

};

#endif
