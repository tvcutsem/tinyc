#ifndef MATCHER_H
#define MATCHER_H

#include "../visitors/codevisitor.h"
#include "../visitors/operatorvisitor.h"
#include "../instructions/register.h"
#include "../operators/operator.h"

using namespace std;

//forward declarations
class NumRegister;
class QCharRegister;

class DividesOp;
class MinusOp;
class PlusOp;
class TimesOp;

class Matcher: public CodeVisitor, public OperatorVisitor { 

  public:

    /**
     * @return true iff the value of reg is neutral for the given operator.
     * @param firstArg is the register denoting the first value of the op or not,
     * this is important for non-commutative operators.
     */
    bool operator()(bool firstArg, Operator& op, Register* reg);

    virtual void visitDividesOp(DividesOp* op);
    
    virtual void visitMinusOp(MinusOp* op);

    virtual void visitPlusOp(PlusOp* op);

    virtual void visitTimesOp(TimesOp* op);

    virtual void visitNumRegister(NumRegister* numreg);

    virtual void visitQCharRegister(QCharRegister* creg);

  private:
    
    bool isFirst_;
    bool operatorOK_;
    bool registerOK_;
    int  neutralElt_;
    int  valueFound_;

};

#endif
