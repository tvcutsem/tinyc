#ifndef OPERATION_H
#define OPERATION_H

#include "../visitors/codevisitor.h"
#include <string>

using namespace std;

/**
 * An Operation is an interface class for denoting 'abstract' operations
 * which can be used by the OperationInstruction class.
 */
class Operation {

  public:

    /**
     * returns the name of this kind of operation
     */
    virtual string getName() const = 0;

    /**
     * accepts a codevisitor
     * this method should call back on the visitor (double dispatch)
     */
    virtual void accept(CodeVisitor* visitor) = 0;

};

#endif
