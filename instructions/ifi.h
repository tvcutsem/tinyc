#ifndef IF_I_H
#define IF_I_H

#include "label.h"
#include "gotoi.h"
#include "varregister.h"
#include "../operators/operator.h"
#include "../visitors/codevisitor.h"

#include <string>

using namespace std;

/**
 * An instruction representing a conditional jump where relop is the
 * relational operator for the condition
 */
class IfI: public GotoI {

  public:

    /**
     * Generate a conditional jump instruction to the given destination
     * @param relOp the releational operator to use
     * @param a the first argument in a relOp b
     * @param b the second argument in a relOp b
     */
    IfI(Operator* relOp,VarRegister* a,VarRegister* b,Label l):
      GotoI(l),relOp_(relOp),firstArg_(a),secondArg_(b) { }


    ~IfI() {
      delete firstArg_;
      delete secondArg_;
    }

    virtual string toString() const {
      return
	"IF "+
	*(firstArg_->name())+" "+
	string(relOp_->symbol())+" "+
	*(secondArg_->name())+" "+
	GotoI::toString();
    }

    VarRegister* getFirstArg() const { return firstArg_; }

    VarRegister* getSecondArg() const { return secondArg_; }

    Operator* getOp() const { return relOp_; }

    void setFirstArg(VarRegister* i) {
      delete firstArg_;
      firstArg_ = i;
    }

    void setSecondArg(VarRegister* i) {
      delete secondArg_;
      secondArg_ = i;
    }

    void setOp(Operator* op) {
      relOp_ = op;
    }

    void accept(CodeVisitor* v) {
      Instruction::accept(v);
      v->visitIfInstruction(this);
    }

  private:
    Operator* relOp_;
    VarRegister* firstArg_;
    VarRegister* secondArg_;

};

#endif
