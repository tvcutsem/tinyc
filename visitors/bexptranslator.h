#ifndef BEXP_IC_TRANSLATOR_H
#define BEXP_IC_TRANSLATOR_H

#include "treevisitor.h"
#include "ictranslator.h"
#include "labelset.h"
#include "../symboltable.h"
#include "../instructions/instructions.h"
#include "../instructions/gotoi.h"
#include "../instructions/ifi.h"
#include "../instructions/varregister.h"
#include "../syntaxtree/number.h"
#include "../syntaxtree/binop.h"

using namespace std;

//forward references

class ICTranslator;

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

/**
 * An Itermediate Code translator which translates only boolean expressions,
 * ie expressions used as conditions in if and while statements.
 * It translates the boolean binary operators by flow of control, rather than
 * an ICOpTranslator which translates them by value.
 */

class BexpTranslator: public TreeVisitor, public OperatorVisitor {

   public:

     /**
      * create a new BexpTranslator 
      * @param parent the ICTranslator who spawned this BexpTranslator to
      * translate his if or while statement
      */

      BexpTranslator(ICTranslator* parent):
		parent_  (parent),
                trueSet_ (new LabelSet),
        	falseSet_(new LabelSet),
		zero_    (new Number(0)) { }

      ~BexpTranslator();

      LabelSet& trueSet()  const { return *trueSet_; }
      LabelSet& falseSet() const { return *falseSet_; }

      /**
       * var
       *  translates to
       * code testing whether the value is zero or not
       */
      virtual void visitVariable(Variable* var);

      /**
       * f(exp1,exp2,exp3)
       *  translates to
       * code testing whether the returnvalue
       * of the function is zero or not
       */
      virtual void visitFuncall(Funcall* funcall);

      /**
       * op exp
       *  translates to
       * code testing whether (op exp) is true or not
       * (if the operator is NOT, this method will just
       * switch the true and false sets)
       */
      virtual void visitUnaryOp(UnaryOp* unop);

      /**
       * exp1 op exp2
       *  translates to
       * code testing whether the result is zero or not
       */
      virtual void visitBinaryOp(BinaryOp* binop);

      /**
       * num
       *  translates to
       * code testing whether num is zero or not
       */
      virtual void visitNumber(Number* num);

      /**
       * char
       *  translates to
       * code testing whether char is zero or not
       */
      virtual void visitQChar(QChar* chr);

      /**
       * length exp
       *  translates to
       * code testing whether the result is zero or not
       */
      virtual void visitLengthExpression(LengthExpression* len);

      /**
       * a[exp1,...,expn]
       *  translates to
       * code testing whether the result is zero or not
       */
      virtual void visitArrayAccess(ArrayAccess* arrayaccess);

      //inherited from operatorvisitor:
      //arithmetic operations will be generated 'by value'
      //boolean operations will be generated 'by flow of control' (in-line)

      virtual void visitDividesOp(DividesOp* op);

      virtual void visitEqualOp(EqualOp* op);

      virtual void visitGreaterOp(GreaterOp* op);

      virtual void visitLessOp(LessOp* op);

      virtual void visitMinusOp(MinusOp* op);

      virtual void visitNEqualOp(NEqualOp* op);

      virtual void visitPlusOp(PlusOp* op);

      virtual void visitTimesOp(TimesOp* op);

   private:

      /**
       * Generates code for an arithmetic binary operation like +,*,...
       */
      void generateArithmeticBinop();

      /**
       * Generates code for a boolean binary operation like <,==,...
       * This can be done 'in-line' in the if statement (by flow of control)
       */
      void generateBooleanBinop();

      /**
       * Indicates the next instruction's label must be added
       * to the true set
       */
      void nextInstrTrue();

      /**
       * Indicates the next instruction's label must be added
       * to the false set
       */
      void nextInstrFalse();

      /**
       * Generate a new register containing 0
       */
      NumRegister* generateZero() const;

      /**
       * Generate code that checks whether the given
       * register contains 0 or not
       */
      void generateZeroTest(VarRegister* var);

      /**
       * Generate code for a jump instruction with
       * yet unknown destination
       */
      void generateZeroJump();

      ICTranslator* parent_;
      LabelSet*     trueSet_;
      LabelSet*     falseSet_;
      Number*       zero_;
      BinaryOp*     currentBinop_;
      
};

#endif
