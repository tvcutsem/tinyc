#ifndef CALL_I_H
#define CALL_I_H

#include "instruction.h"
#include "funregister.h"
#include "../visitors/codevisitor.h"

#include <string>

using namespace std;

/**
 * A Call instruction calls a function with the arguments on top of stack
 */

class CallI: public Instruction {

  public:
    CallI(FunRegister* fun, int nrOfArgs);

    ~CallI();

    virtual string toString() const;

    FunRegister* getFunction() const;
    
    int getNrOfArgs() const;

    void setFunRegister(FunRegister* newVal);

    void setNrOfArgs(int nrOfArgs);

    void accept(CodeVisitor* v) {
      Instruction::accept(v);
      v->visitCallInstruction(this);
    }

  private:
    FunRegister* function_;
    int nrOfArgs_;
  
};

#endif
