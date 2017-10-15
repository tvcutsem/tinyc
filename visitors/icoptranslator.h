#ifndef IC_OPERATOR_TRANSLATOR_H
#define IC_OPERATOR_TRANSLATOR_H

#include "operatorvisitor.h"
#include "ictranslator.h"
#include "../syntaxtree/number.h"
#include "../instructions/varregister.h"
#include "../instructions/numregister.h"
#include "../instructions/ifi.h"
#include "../type.h"

//forward declarations

class OperatorVisitor;

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
 * An ICOpTranslator is used by an ICTranslator to translate
 * Unary and Binary operators. Its primary function is to
 * translate relational operators by value.
 */

class ICOpTranslator: public OperatorVisitor {

  public:

    ICOpTranslator(ICTranslator& parent):
      parent_(parent),
      zero_  (new Number(0)),
      one_   (new Number(1)) { }

    ~ICOpTranslator() {
      delete zero_;
      delete one_;
    }

    //accessors
    Type resultType()        const { return resultType_; }
    VarRegister* firstArg()  const { return arg1_; }
    VarRegister* secondArg() const { return arg2_; }
    VarRegister* result()    const { return result_; }
    
    //mutators
    void setResultType(Type newType)       { resultType_ = newType; }
    void setFirstArg(VarRegister* newReg)  { arg1_ = newReg; }
    void setSecondArg(VarRegister* newReg) { arg2_ = newReg; }

    virtual void visitDividesOp(DividesOp* op);
    
    virtual void visitEqualOp(EqualOp* op);

    virtual void visitGreaterOp(GreaterOp* op);

    virtual void visitLessOp(LessOp* op);

    virtual void visitMinusOp(MinusOp* op);

    virtual void visitNEqualOp(NEqualOp* op);

    virtual void visitPlusOp(PlusOp* op);

    virtual void visitTimesOp(TimesOp* op);

    virtual void visitUMinusOp(UMinusOp* op);
    
    virtual void visitNegateOp(NegateOp* op);

  private:

    /**
     * Generate a test and branch instruction with yet unknown destination
     */
    IfI* generateTest(Operator* op,VarRegister* arg1, VarRegister* arg2);

    /**
     * Generate the constant 0 (boolean false)
     */
    NumRegister* makeZero();

    /**
     * Generate the constant 1 (boolean true)
     */
    NumRegister* makeOne();

    /**
     * Generates default code for arithmetic operations like +,*,-,/, ...
     */
    void generateArithMeticOp(Operator* op);

    /**
     * Generates default code for boolean operations like <,>,!=,==, ...
     */
    void generateBooleanOp(Operator* op);

    ICTranslator& parent_;
    Number* zero_; /**< the constant zero (false) */
    Number* one_;  /**< the constant one (true) */
    
    Type resultType_;     /**< the type of Op(arg1_,arg2_) */
    VarRegister* arg1_;   /**< first argument of the operator */
    VarRegister* arg2_;   /**< second argument of the operator */
    VarRegister* result_; /**< the register that contains the result */

};

#endif
