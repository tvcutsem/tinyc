#ifndef WRITE_INSTRUCTION_H
#define WRITE_INSTRUCTION_H

#include "opinstruction.h"
#include "varregister.h"
#include "../visitors/codevisitor.h"

#include <string>

using namespace std;

class WriteI;

/**
 * The implementation of the operation interface for write instructions
 */
class WriteOperation: public Operation {

  public:
    
    WriteOperation(WriteI* writei): writei_(writei) { }

    string getName() const { return "WRITE"; }

    void accept(CodeVisitor* v) { v->visitWriteInstruction(writei_); }

  private:
    WriteI* writei_;
};


/**
 * A Write Instruction which can write to the terminal and store the
 * result in a variable
 */
class WriteI: public OperationI {

  public:
    WriteI(VarRegister* source):
      	OperationI(new WriteOperation(this),source) { }
    
};

#endif
