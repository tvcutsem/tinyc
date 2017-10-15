#ifndef RETVAL_INSTRUCTION_H
#define RETVAL_INSTRUCTION_H

#include "assigni.h"
#include "varregister.h"
#include "../visitors/codevisitor.h"

#include <string>

using namespace std;

/**
 * A Return Value instruction is usually generated after a CALL instruction
 * to load the return value of a function into a register.
 */
class RetValI: public AssignmentI {
  public:

    /**
     * Creates an instruction A = RETVAL
     */
    RetValI(VarRegister* target): AssignmentI(target) { }

    string toString() const {
      return AssignmentI::toString() + "RETVAL";
    }

    void accept(CodeVisitor* v) {
      Instruction::accept(v);
      v->visitReturnValInstruction(this);
    }

};

#endif
