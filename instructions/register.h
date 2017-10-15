#ifndef REGISTER_H
#define REGISTER_H

#include "../visitors/codevisitor.h"

#include <string>

using namespace std;

/**
 * A register is an abstraction used by three address code to represent
 * identifiers.
 */
class Register {

  public:

    Register(bool global): isGlobal_(global) { }

    virtual ~Register() { }

    /**
     * Returns the name of the identifier represented by this register
     */
    virtual string* name() const = 0;

    /**
     * Accept a CodeVisitor for double dispatch
     */
    virtual void accept(CodeVisitor* v) = 0;

    /**
     * Is this register containing a temporary variable or not?
     */
    virtual bool isTemporary() const { return false; }

    /**
     * Is this register containing a constant or not?
     */
    virtual bool isConstant() const { return false; }

    bool operator<(Register& other) {
      return ((*name()) < (*other.name()));
    }

    bool operator==(Register& other) {
      return ((*name()) == (*other.name()));
    }

    /**
     * @return whether this register is global or not
     */
    bool isGlobal() const { return isGlobal_; }

  private:

    bool isGlobal_; /**< is this register global or not? */

};

#endif
