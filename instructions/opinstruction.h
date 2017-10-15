#ifndef OPERATION_INSTRUCTION_H
#define OPERATION_INSTRUCTION_H

#include "instruction.h"
#include "operation.h"
#include "varregister.h"
#include "../visitors/codevisitor.h"

#include <string>

using namespace std;

/**
 * An Operation Instruction is the superclass of all instructions who
 * take one target register and perform an operation onto it, storing
 * the result in another one
 */
class OperationI: public Instruction {

  public:
    OperationI(Operation* o, VarRegister* source);
    ~OperationI();

    virtual string toString() const;

    //accessors
    VarRegister* getSource()    const { return source_; }
    Operation*   getOperation() const { return op_; }
    
    //mutators
    void setSource(VarRegister* source) {
      delete source_;
      source_ = source;
    }

    void setOperation(Operation* op) {
      delete op_;
      op_ = op;
    }

    virtual void accept(CodeVisitor* v);
    
  private:
    Operation* op_;
    VarRegister* source_;
  
};

#endif
