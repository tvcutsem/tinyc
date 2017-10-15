#ifndef SIMPLE_ASSIGN_I_H
#define SIMPLE_ASSIGN_I_H

#include "instruction.h"
#include "varregister.h"
#include "../visitors/codevisitor.h"

#include <string>

using namespace std;

/**
 * A copy instruction from one variable to another one
 */
class SimpleAssignmentI: public AssignmentI {

  public:

    SimpleAssignmentI(VarRegister* target,VarRegister* source):
      	AssignmentI(target),source_(source) { }

    ~SimpleAssignmentI() { delete source_; }

    /**
     * Returns the source variable 
     */
    VarRegister* getSource() const { return source_; }

    void setSource(VarRegister* source) {
      delete source_;
      source_ = source;
    }

    string toString() const {
      return AssignmentI::toString() + *(source_->name());
    }

    void accept(CodeVisitor* v) {
      Instruction::accept(v);
      v->visitSimpleAssInstruction(this);
    }

  private:
    VarRegister* source_;
  
};

#endif
