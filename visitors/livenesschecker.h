#ifndef LIVENESS_CHECKER_H
#define LIVENESS_CHECKER_H

#include "treevisitor.h"
#include "variableset.h"
#include "livenessexc.h"

//forward references

class Variable;
class Function;
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
class LengthExpression;
class ArrayAccess;

/**
 * A LivenessChecker does three things:
 *  - check whether there are variables which are never used.
 *  - check whether there are variables which are used before
 *    they are initialized.
 *  - check whether we reach the end of a non-void function.
 *
 * A LivenessChecker uses VariableSets to keep track of uninitialized variables.
 *
 * Imagine the program being as follows:
 *
 *  {
 *   int x;
 *   int y;
 *   int z;
 *   ...
 *   y = ...
 *   ...
 *   if(...) {
 *     x = ...
 *     ...
 *   } else {
 *     ...
 *   }
 *   ...x... //although x is used here, it might not have
 *  }        //been initialized because the if might have failed
 *
 *  The checker first initializes a so called 'uninitialized' set to
 *  all variables declared in the block. Thus the set starts out as
 *  { x, y, z } 
 *
 *  When the checker sees the assignment to y, it marks y as 'initialized' by
 *  removing it from the set, thus, when the checker reaches the if, the set
 *  is now: { x, z }
 *
 *  The checker goes on by checking both branches of the if, both branches
 *  are checked with respect to (a copy of) the set { x, z }
 *
 *  The true branch will 'define' x, and thus, at the end of this branch
 *  the set is down to { z }
 *
 *  The else branch does not 'define' anything, so the set comes out unchanged
 *  as { x, z }
 *
 *  Then, both sets are combined using a logical OR (or a union in set theory)
 *  The reasoning behind this is simple:
 *  if a variable has been defined in only one branch, it MIGHT still have
 *  not been initialized in the code following the if, so the only way a variable
 *  cannot be in the union of both branches' sets is by being defined in both of them.
 *
 *  Continuing our example, the union of { z } and { x, z } is { x, z }
 *  
 *  When the checker reaches the use of x, below, it will check whether x
 *  is in the set of 'uninitialized' variables, this is the case, so an error
 *  is reported (something like: 'x is used before it is initialized')
 *
 *  Reaching the end of the function, the compiler will also throw an error for each
 *  variable still in the set of unused variables (this is another set used
 *  to keep track of all variables which were never used in the function),
 *
 *  This checker thus catches both uninitialized and unused variables.
 *  A third and very cool utility of this checker is checking whether
 *  a function ever reaches the end of it's control flow without a return (if the
 *  return type is not void). This is done as follows:
 *  
 *  At the beginning of the function, a dummy RETURN variable is inserted in
 *  the uninitialized set, so the set starts out as { RETURN }
 *
 *  Now, when visiting a 'return' statement, we just remove the RETURN variable,
 *  as if it was 'initialized'. At the end of the function, if the RETURN is still
 *  in the set, the function might fall of the end of it's code. Very nifty :)
 *
 *  Note that arrays are considered 'initialized' when they are created,
 *  so there is no need to insert array variables in the uninitialized set
 */

class LivenessChecker: public TreeVisitor {

   public:

     LivenessChecker();

     ~LivenessChecker();

     /**
      * Check an identifier and dump all errors in the given error list
      * (only functions will be checked)
      */
     void check(Identifier* id, ControlErrors& errors); 

      /**
       * When a variable is visited, if var is an element of the uninitialized set,
       * report an 'uninitialized error'
       */
      virtual void visitVariable(Variable* var);

      /**
       * Add the RETURN variable to the uninitialized set.
       * Visit the block.
       * Generate a warning for each variable still in the uninitialized set.
       */
      virtual void visitFunction(Function* fun);
      
      /**
       * When visiting the block, add all variables declared in it to the
       * set of uninitialized variables.
       */
      virtual void visitBlock(Block* b);

      /**
       * When visiting an if, visit both branches with a copy
       * of the uninitialized set.
       * After visiting them, set the uninitialized set to the
       * union of both resulting sets.
       */
      virtual void visitIfStatement(IfStatement* ifstmt);

      /**
       * When visiting a while, visit the body with a copy of the
       * uninitialized set. But, when continuing after the while,
       * just discard the set with which the while was visited.
       *
       * This means: any 'definition' made in a while is useless for the
       * rest of the code, which makes sense:
       * 
       * int x;
       * while (...) {
       *   x = ...
       * }
       * ... x ... //no guarantee that x has been initialized
       *
       * So after the while, we continue with all uninitialized variables
       * from before the while (by the way, this also applies to if statements
       * without an else branch, you can also regard this as the union of both
       * if branches, where one branch is unmodified (it's empty so doesn't define
       * any variables), so the union is always the original set).
       */
      virtual void visitWhileStatement(WhileStatement* whilestmt);

      /**
       * When visiting an assignment, remove the target variable
       * from the uninitialized list
       */
      virtual void visitAssignment(Assignment* assgn);

      /**
       * Remove the dummy RETURN variable from the uninitialized list.
       */
      virtual void visitReturnStatement(ReturnStatement* ret);

      /**
       * Propagate and check for uninitialized used variables...
       */
      virtual void visitFuncall(Funcall* funcall);

      /**
       * Treat the target of the read as if it was the target
       * of an assignment, in other words: remove it from the
       * uninitialized set.
       */
      virtual void visitReadStatement(ReadStatement* readstmt);

      /**
       * Propagate and check for uninitialized used variables...
       */
      virtual void visitWriteStatement(WriteStatement* writestmt);

      /**
       * Propagate and check for uninitialized used variables...
       */
      virtual void visitUnaryOp(UnaryOp* unop);

      /**
       * Propagate and check for uninitialized used variables...
       */
      virtual void visitBinaryOp(BinaryOp* binop);

      /**
       * Propagate and check for uninitialized used variables...
       */
      virtual void visitLengthExpression(LengthExpression* len);

      /**
       * Propagate and check for uninitialized used variables...
       */
      virtual void visitArrayAccess(ArrayAccess* arrayaccess);

   private:

      /**
       * A status field is used to mark whether we are currently
       * checking a variable (for usage before it was initialized)
       * or whether we are initializing it.
       *
       * Eg, when visiting
       *   x = y + z;
       * When checking the expression y + z, status is CHECKING_VAR,
       * but when visiting the left-hand side, status is INITIALIZING_VAR
       *
       * see visitVariable.
       */
      enum Status { INITIALIZING_VAR, CHECKING_VAR };

      /** set of uninitialized variables */
      VariableSet* uninitialized_;

      /** set of unused variables */
      VariableSet* unused_;

      /** the dummy return variable */
      Variable* returnVar_;

      Status status;

      /** the list of errors to write checking errors to */
      ControlErrors* errors_;

};

#endif
