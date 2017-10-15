#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "instruction.h"
#include "label.h"

#include <vector>

/**
 * Instructions represents a stream of three address code instructions
 */
class Instructions: public vector<Instruction*> {

  public:

    Instructions(): nextLabel_(0) { }

    /**
     * An instructionstream deletes its instruction parts!
     */
    ~Instructions();

    friend ostream& operator<<(ostream& os,Instructions& instructions);

    /**
     * Returns the instruction with the specified label.
     * A null pointer is returned if there is no such instruction.
     */
    Instruction* operator[](const Label l) const;

    /**
     * Returns the label the next instruction appended to this stream
     * will receive
     */
    Label nextInstructionLabel() const;
    
    /**
     * Appends an instruction to the codestream.
     * Automatically sets the label of this instruction.
     */
    Instructions& operator<<(Instruction* i);

  private:
    Label nextLabel_;

};

typedef Instructions::iterator Instructionit;

typedef Instructions::const_iterator ConstInstructionit;

ostream& operator<<(ostream& os,Instructions& instructions);

#endif
