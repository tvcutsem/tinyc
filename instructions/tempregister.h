#ifndef TEMP_REGISTER_H
#define TEMP_REGISTER_H

#include "varregister.h"
#include "../syntaxtree/variable.h"

#include <string>

using namespace std;

/**
 * A VarRegister is a placeholder for a variable
 */
class TempRegister: public VarRegister {
  public:

    TempRegister(Variable* var): VarRegister(var) { }

    /**
     * Construct a tempregister out of a varregister.
     */
    TempRegister(VarRegister& var): VarRegister(var.name(),var.type()) { }
    
    virtual bool isTemporary() const { return true; }

};

#endif
