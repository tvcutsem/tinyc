#ifndef NUM_REGISTER_H
#define NUM_REGISTER_H

#include "varregister.h"
#include "../type.h"
#include "../stringpool.h"
#include "../syntaxtree/number.h"
#include "../visitors/codevisitor.h"

#include <string>
#include <sstream>

using namespace std;

/**
 * A NumRegister is a placeholder for a constant.
 * It can be used in a three address instruction.
 */
class NumRegister: public VarRegister {
  public:

    /**
     * Construct a NumRegister out of a Number
     */
    NumRegister(Number* num): VarRegister(num2str(num->getValue()),int_t),
    			      number_(num->getValue()) { }

    /**
     * Construct a NumRegister out of an integer value
     */
    NumRegister(int value): VarRegister(num2str(value),int_t),
                            number_(value) { }


    /**
     * The value of this constant
     */
    int value() const { return number_; }

    virtual void accept(CodeVisitor* v) {
      v->visitNumRegister(this);
    }

    virtual bool isConstant() const { return true; }

  private:
    string* num2str(int number) const {
      stringstream ss;
      ss << number;
      return StringPool::instance()->getString(ss.str());
    }

    int number_;

};

#endif
