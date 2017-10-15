#ifndef OPERATOR_VISITOR_H
#define OPERATOR_VISITOR_H

//forward declarations
class DividesOp;
class EqualOp;
class GreaterOp;
class LessOp;
class MinusOp;
class NEqualOp;
class PlusOp;
class TimesOp;
class UMinusOp;
class NegateOp;

/**
 * An operatorvisitor can act upon the specific type of the operator.
 * This is used for eg code generation.
 */

class OperatorVisitor {

  public:

    /**
     * Virtual destructor.
     * This class will be subclassed.
     */
    virtual ~OperatorVisitor() { }

    virtual void visitDividesOp(DividesOp* op) { }
    
    virtual void visitEqualOp(EqualOp* op) { }

    virtual void visitGreaterOp(GreaterOp* op) { }

    virtual void visitLessOp(LessOp* op) { }

    virtual void visitMinusOp(MinusOp* op) { }

    virtual void visitNEqualOp(NEqualOp* op) { }

    virtual void visitPlusOp(PlusOp* op) { }

    virtual void visitTimesOp(TimesOp* op) { }

    virtual void visitUMinusOp(UMinusOp* op) { }

    virtual void visitNegateOp(NegateOp* op) { }

};

#endif
