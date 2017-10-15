#ifndef ADDRESS_I_H
#define ADDRESS_I_H

#include "assigni.h"
#include "varregister.h"
#include "arrayregister.h"
#include "../visitors/codevisitor.h"

#include <string>

using namespace std;

/**
 * An instruction taking the address of a variable
 * @deprecated replaced by ArrayParamI for its purpose of passing an array by address
 */
class AddressI: public AssignmentI {

  public:

    /**
     * Creates an instruction A = address B, where A is the target
     * and B is the source
     */
    AddressI(VarRegister* target, ArrayRegister* source):
      AssignmentI(target),source_(source) { }

    ~AddressI() { delete source_; }

    /**
     * Returns the variable from which the address will be taken 
     */
    ArrayRegister* getSource() const { return source_; }

    void setSource(ArrayRegister* source) {
      delete source_;
      source_ = source;
    }

    string toString() const {
      return
	AssignmentI::toString()+
	"ADDRESS "+
	*(source_->name());
    }

    void accept(CodeVisitor* v) {
      Instruction::accept(v);
      v->visitAddressInstruction(this);
    }

  private:
    ArrayRegister* source_;
  
};

#endif
