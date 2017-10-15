#ifndef IC_TRANSLATOR_H
#define IC_TRANSLATOR_H

#include "../symboltable.h"
#include "../instructions/instructions.h"
#include "../syntaxtree/identifier.h"
#include "../syntaxtree/aggregates.h"
#include "bexptranslator.h"

using namespace std;

//forward references

class ICOpTranslator;

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
class BreakStatement;
class ContinueStatement;

/**
 * An Itermediate Code translator translates the abstract syntax tree to
 * three address code
 */

class ICTranslator: public TreeVisitor {

   public:

     /**
      * Create a new intermediate code translator
      * @param codestream the stream to which the code should be written
      * @param globalScope the global symbol table
      */

      ICTranslator(Instructions* codestream,SymbolTable* globalScope);

      ~ICTranslator();

      Instructions* getCodeStream()  const { return &codestream_; }

      Identifiers* getDeclarations() const { return declarations_; }
     
      /**
       * var
       *  translates to
       * var
       * place_ will be set to var
       */
      virtual void visitVariable(Variable* var);

      /**
       * fun
       *  translates to
       * fun
       *  (code will be generated for the function body)
       */
      virtual void visitFunction(Function* fun);

      /**
       * array
       *  translates to
       * one dimensional array
       * place_ will be set to the array
       * Multi-dimensional arrays are translated to one-dimensional arrays
       */
      virtual void visitArray(Array* array);
      
      /**
       * block
       *  translates to
       * create a set of instructions for all statements in the block
       */
      virtual void visitBlock(Block* b);

      /**
       * if bexp then stmt1 else stmt2
       *  translates to
       * generate code for bexp
       * generate code for stmt1
       * generate code for escaping stmt2
       * generate code for stmt2
       * adjust true and false sets of bexp accordingly
       * (bexp is evaluated by flow of control)
       * 
       * BEXP - true -+
       *  |	      |
       * false        |
       *  v           |
       * goto ---+    |
       *         |    |
       * stmt1 <-+----+
       *         |    
       * goto ---+----+
       *         |    |
       * stmt2 <-+    |
       *       <------+
       */
      virtual void visitIfStatement(IfStatement* ifstmt);

      /**
       * while bexp stmt
       *  translates to
       * generate code for bexp
       * generate code for stmt
       * GOTO bexp
       * adjust true and false sets of bexp accordingly
       * (bexp is evaluated by flow of control)
       *
       * BEXP - false -+
       *  |	<---+  |
       * true       |  |
       *  v         |  |
       * stmt1      |  |
       *            |  |
       * goto ------+  |
       *               |
       *     <---------+
       */
      virtual void visitWhileStatement(WhileStatement* whilestmt);

      /**
       * var = exp
       *  translates to
       * t1 = ... //value of exp
       * var = t1
       */
      virtual void visitAssignment(Assignment* assgn);

      /**
       * return exp
       *  translates to
       * t1 = ... //value of exp
       * RETURN t1
       */
      virtual void visitReturnStatement(ReturnStatement* ret);

      /**
       * f(exp1,exp2,exp3)
       *  translates to
       * t1 = ... //value of exp1
       * t2 = ... //value of exp2
       * t3 = ... //value of exp3
       * PARAM t1
       * PARAM t2
       * PARAM t3
       * CALL f,3
       */
      virtual void visitFuncall(Funcall* funcall);

      /**
       * read exp
       *  translates to
       * t1 = ... //value of exp
       * READ t1
       */
      virtual void visitReadStatement(ReadStatement* readstmt);

      /**
       * write exp
       *  translates to
       * t1 = ... //value of exp
       * WRITE t1
       */
      virtual void visitWriteStatement(WriteStatement* writestmt);

      /**
       * op exp
       *  translates to
       * t1 = ... //value of exp
       * t2 = op t1
       */
      virtual void visitUnaryOp(UnaryOp* unop);

      /**
       * exp1 op exp2
       *  translates to
       * t1 = ... //value of exp1
       * t2 = ... //value of exp2
       * t3 = t1 op t2
       *
       * Note that '<', '>', ... will evaluate booleans by value
       */
      virtual void visitBinaryOp(BinaryOp* binop);

      /**
       * num
       *  translates to
       * a variable representing this constant
       */
      virtual void visitNumber(Number* num);

      /**
       * char
       *  translates to
       * a variable representing this constant
       */
      virtual void visitQChar(QChar* chr);

      /**
       * length exp
       *  translates to
       * t1 = ... //value of exp
       * t2 = LENGTH t1
       */
      virtual void visitLengthExpression(LengthExpression* len);

      /**
       * a[exp1,...,expn]
       *  translates to
       * t1 = ... //value of exp1
       * tn = ... //value of expn
       * tn+1 = fold indices into one index for a one-dimensional array
       * tn+2 = a[tn+1]
       */
      virtual void visitArrayAccess(ArrayAccess* arrayaccess);

      /**
       * break;
       *  translates to
       * GOTO false-exit of the WHILE loop
       */
      virtual void visitBreakStatement(BreakStatement* breakstmt);

      /**
       * continue;
       *  translates to
       * GOTO true-exit of the WHILE loop
       */
      virtual void visitContinueStatement(ContinueStatement* ctustmt);

      /**
       * BexpTranslator can access the codestream and scope if necessary
       */
      friend class BexpTranslator;

      /**
       * An ICOpTranslator can also write to my codestream
       */
      friend class ICOpTranslator;

   private:

      /**
       * Fold the indices of a multi-dimensional array into one index
       * for a one-dimensional array.
       * @return the temporary variable containing the folded index
       */
      VarRegister* foldIndex(Array* array, Expressions& indices);

      /**
       * (presumably) safe downcast to a variable
       */
      Variable* lexp2variable(LExpression* lexp);

      /**
       * (presumably) safe downcast to an array access
       */
      ArrayAccess* lexp2array(LExpression* lexp);

      /**
       * Qualifies a name with the functionname or block scopes
       * to make it unique
       */
      string* qualifyName(string* name) const;

      /**
       * Generate code for retrieving the dimensions of a multidimensional
       * array at run-time
       */
      void generateMultidimOffsets(SymbolTable& scope,
	                           Instructions& instrstream, Array& array);
      
      //INSTANCE VARIABLES

      /** the stream to which instructions are written */
      Instructions& codestream_;

      /** the target of a previously generated assignment */
      VarRegister* place_;

      /** scope of the current function */
      SymbolTable* scope_;

      /** helper visitor for bexp's */
      ICOpTranslator* myOpTranslator_;

      /** current nesting level of blocks */
      int nestingLevel_;

      /** the current function's name */
      string functionName_;

      /** declarations in current function */
      Identifiers* declarations_;

      /** current bexptranslator, if there is any */
      BexpTranslator* currentBexpTranslator_;

      /**
       * label holding the beginning of the while which is currently
       * being visited (only makes sense if currentBexpTranslator
       * is non null)
       */
      Label currentWhileBeginLabel_;
      
};

#endif
