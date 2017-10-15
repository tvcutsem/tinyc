#ifndef VAR_REGISTER_H
#define VAR_REGISTER_H

#include "register.h"
#include "../type.h"
#include "../stringpool.h"
#include "../syntaxtree/variable.h"
#include "../visitors/codevisitor.h"

#include <string>
#include <sstream>
#include <iostream>

using namespace std;

/**
 * A VarRegister is a placeholder for a variable
 */
class VarRegister: public Register {

  public:

    /**
     * Construct a VarRegister out of a Variable
     */
    VarRegister(Variable* var): Register(var->isGlobal()),
    				name_(var->name()),type_(var->type()) { }

    virtual ~VarRegister() { }

    /**
     * The name of this variable
     */
    virtual string* name() const { return name_; }

    /**
     * The type of this variable
     */
    Type type() const { return type_; }
    
    virtual void accept(CodeVisitor* v) {
      v->visitVarRegister(this);
    }

  protected:
    //constructor for the children
    VarRegister(string* name,Type type):
       Register(false), name_(name),type_(type) { }

  private:
    string* name_;
    Type type_;

};

#endif
