#ifndef UNARY_ASSIGN_I_H
#define UNARY_ASSIGN_I_H

#include "assigni.h"
#include "varregister.h"
#include "../operators/operators.h"
#include "../visitors/codevisitor.h"

#include <string>

using namespace std;

/**
 * An instruction that takes one argument and a unary operation, and which
 * then assigns the result of unop(arg) to another variable
 */
class UnAssignmentI: public AssignmentI {

  public:

    /**
     * @param op the operator to use 
     * @param target the variable where the result will be stored
     * @param arg the argument to the unary operator
     */
    UnAssignmentI(Operator* op,VarRegister* target,VarRegister* arg);

    ~UnAssignmentI();

    string toString() const;

    //accessors
    Operator* getOperator() const { return unOp_; }

    VarRegister* getArg() const { return argument_;; }

    //mutators
    void setOperator(Operator* op) { unOp_ = op; }

    void setArg(VarRegister* i) {
      delete argument_;
      argument_ = i;
    }

    void accept(CodeVisitor* v) {
      Instruction::accept(v);
      v->visitUnaryAssInstruction(this);
    }

  private:
    Operator* unOp_;
    VarRegister* argument_;
  
};

#endif
