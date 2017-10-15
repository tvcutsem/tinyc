#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "label.h"
#include "../visitors/codevisitor.h"

#include <string>
#include <iostream>

using namespace std;

/**
 * An Instruction is the superclass of all three address instructions, which is
 * the intermediate code of the tiny compiler.
 */
class Instruction {

  public:
    Instruction(): label_(0), isLeader_(false) { }
    
    virtual ~Instruction() { } 

    /**
     * Returns the label (position) of this instruction
     */
    Label getLabel() const { return label_; }

    /**
     * Sets the label of this instruction
     */
    void setLabel(Label newLabel) { label_ = newLabel; }

    /**
     * Returns a printable representation of this instruction
     */
    virtual string toString() const = 0;

    /**
     * Accepts a CodeVisitor. This method should call back on the visitor
     * itself. It implements a double dispatch.
     */
    virtual void accept(CodeVisitor* v) {
        //allows visitors to take special actions when visiting a leader
      	if (isLeader())
	  v->visitLeader(this);
    };

    /**
     * Denotes that this instruction is a 'leader' in a basic block
     */
    void setLeader() { isLeader_ = true; }
    
    /**
     * Returns whether this instruction is a 'leader' in a basic block
     */
    bool isLeader() const { return isLeader_; }

    friend ostream& operator<<(ostream& os,Instruction* i);
  private:
    Label label_;
    bool isLeader_;
};

ostream& operator<<(ostream& os, Instruction* i);

#endif
