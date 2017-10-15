#ifndef BIN_ASSIGN_I_H
#define BIN_ASSIGN_I_H

#include "assigni.h"
#include "varregister.h"
#include "../syntaxtree/identifier.h"
#include "../operators/operators.h"
#include "../visitors/codevisitor.h"

#include <string>

using namespace std;

/**
 * An instruction that takes two arguments and a binary operation, and which
 * then assigns the result to another variable
 */
class BinAssignmentI: public AssignmentI {

  public:

    /**
     * @param op the operator to combine the arguments
     * @param target the variable where the result will be stored
     * @param a the first argument to op
     * @param b the second argument to op
     */
    BinAssignmentI(Operator* op,VarRegister* target,VarRegister* a,VarRegister* b);

    ~BinAssignmentI();

    string toString() const;

    //accessors
    Operator*    getOperator()  const { return binOp_; }
    VarRegister* getFirstArg()  const { return firstArg_; }
    VarRegister* getSecondArg() const { return secondArg_; }

    //mutators
    void setOperator(Operator* op) { binOp_ = op; }
    void setFirstArg(VarRegister* i) { firstArg_ = i; }
    void setSecondArg(VarRegister* i) { secondArg_ = i; }

    void accept(CodeVisitor* v) {
      Instruction::accept(v);
      v->visitBinaryAssInstruction(this);
    }

  private:
    Operator*    binOp_;
    VarRegister* firstArg_;
    VarRegister* secondArg_;
  
};

#endif
