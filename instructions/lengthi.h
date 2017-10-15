#ifndef LENGTH_INSTRUCTION_H
#define LENGTH_INSTRUCTION_H

#include "assigni.h"
#include "varregister.h"
#include "arrayregister.h"

#include <string>

using namespace std;
 
/**
 * A Length Instruction can take the length of an array and store
 * the result in a new variable
 */
class LengthI: public AssignmentI {

  public:

    /**
     * Creates a new Length Instruction of the form A = LENGTH(B)
     * where A is the target and B is the source array
     */
    LengthI(VarRegister* target, ArrayRegister* source):
      AssignmentI(target),source_(source) { }

    ~LengthI() {
      delete source_;
    }

    ArrayRegister* getSource() const { return source_; }

    void setSource(ArrayRegister* source) { source_ = source; }

    string toString() const {
      return AssignmentI::toString() +"LENGTH " + *(source_->name());
    }

    void accept(CodeVisitor* v) {
      Instruction::accept(v);
      v->visitLengthInstruction(this);
    }

  private:
    ArrayRegister* source_;
 
};

#endif
