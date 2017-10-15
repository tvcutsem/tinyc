#ifndef CHAR_REGISTER_H
#define CHAR_REGISTER_H

#include "register.h"
#include "../type.h"
#include "../stringpool.h"
#include "../syntaxtree/qchar.h"
#include "../visitors/codevisitor.h"

#include <string>
#include <sstream>

using namespace std;

/**
 * A QCharRegister is a placeholder for a constant.
 * It can be used in a three address instruction.
 */
class QCharRegister: public VarRegister {
  public:

    /**
     * Construct a QCharRegister out of a QChar
     */
    QCharRegister(QChar* qchar): VarRegister(qchar2str(qchar->getValue()),char_t),
    				 qchar_(qchar->getValue()) { }

    /**
     * The value of this constant
     */
    char value() const { return qchar_; }

    virtual void accept(CodeVisitor* v) {
      v->visitQCharRegister(this);
    }

    virtual bool isConstant() const { return true; }

  private:

    string* qchar2str(char qchar) const {
      stringstream ss;
      ss << qchar;
      return StringPool::instance()->getString(ss.str());
    }

    char qchar_;

};

#endif
