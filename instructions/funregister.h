#ifndef FUN_REGISTER_H
#define FUN_REGISTER_H

#include "register.h"
#include "../type.h"
#include "../stringpool.h"
#include "../syntaxtree/function.h"
#include "../visitors/codevisitor.h"

#include <string>
#include <sstream>
#include <iterator>
#include <algorithm>

using namespace std;

/**
 * A FunRegister is a placeholder for a function
 */
class FunRegister: public Register {

  public:

    /**
     * Construct a FunRegister out of a Function
     */
    FunRegister(Function* fun): Register(true),
    				name_(fun->name()),returnType_(fun->returnType()) {
      //copy all formal types to the typelist
      Identifiers formals;
      fun->parameters(formals);
      formals_ = new Identifiers(formals);
    }

    /**
     * Simple destructor (gang of three)
     */
    ~FunRegister() {
      delete formals_;
    }

    /**
     * Customized copy constructor (gang of three)
     */
    FunRegister(FunRegister& other): Register(true) {
      name_ = other.name_;
      returnType_ = other.returnType_;
      formals_ = new Identifiers(*other.formals_);
    }

    /**
     * The name of this function
     */
    virtual string* name() const { return name_; }

    /**
     * The returntype of this function
     */
    Type returnType() const { return returnType_; }

    /**
     * The argument types of this function
     */
    Identifiers* formals() const { return formals_; }
    
    virtual void accept(CodeVisitor* v) {
      v->visitFunRegister(this);
    }

  private:
    string* name_;
    Type returnType_;
    Identifiers* formals_;

};

#endif
