#ifndef GOTO_I_H
#define GOTO_I_H

#include "label.h"
#include "instruction.h"
#include "../visitors/codevisitor.h"

#include <string>

using namespace std;

/**
 * An instruction representing an unconditional jump
 */
class GotoI: public Instruction {

  public:
    /**
     * Generate a jump instruction to the given destination
     */
    GotoI(Label l): destination_(l) { }

    /**
     * Returns the label to which the jump instruction will jump
     */
    Label getDestination() const { return destination_; }

    void setDestination(Label dest) { destination_ = dest; }

    virtual string toString() const {
      return string("GOTO ")+label2str(destination_);
    }

    void accept(CodeVisitor* v) {
      Instruction::accept(v);
      v->visitGotoInstruction(this);
    }

  private:
    Label destination_;

};

#endif
