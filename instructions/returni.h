#ifndef RETURN_I_H
#define RETURN_I_H

#include "varregister.h"
#include "instruction.h"
#include "../visitors/codevisitor.h"

#include <string>

using namespace std;

/**
 * A Return instruction returns an argument to the caller 
 */
class ReturnI: public Instruction {

  public:
    ReturnI(VarRegister* val);

    ~ReturnI();

    virtual string toString() const;

    VarRegister* getValue() const;

    void setValue(VarRegister* newVal);

    void accept(CodeVisitor* v) {
      Instruction::accept(v);
      v->visitReturnInstruction(this);
    }

  private:
    VarRegister* value_;
  
};

#endif
