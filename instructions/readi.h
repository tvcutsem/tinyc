#ifndef READ_INSTRUCTION_H
#define READ_INSTRUCTION_H

#include "opinstruction.h"
#include "varregister.h"
#include "../visitors/codevisitor.h"

#include <string>

using namespace std;

class ReadI;

/**
 * The implementation of the operation interface for read instructions
 */
class ReadOperation: public Operation {

  public:
    
    ReadOperation(ReadI* readi): readi_(readi) { }

    string getName() const { return "READ"; }

    void accept(CodeVisitor* v) { v->visitReadInstruction(readi_); }

  private:
    ReadI* readi_;
};

/**
 * A Read Instruction which can read from the terminal and store the
 * result in a variable
 */
class ReadI: public OperationI {

  public:
    ReadI(VarRegister* target):
      	OperationI(new ReadOperation(this),target) { }
    
};

#endif
