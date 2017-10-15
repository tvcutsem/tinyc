#ifndef MULTI_DIM_H
#define MULTI_DIM_H

#include "instruction.h"
#include "../visitors/codevisitor.h"
#include "reglist.h"
#include "varregister.h"
#include "numregister.h"
#include "arrayregister.h"

#include <string>
#include <iostream>

using namespace std;

/**
 * A MULTIDIM instruction is used to declare a multi-dimensional array at
 * run time.
 *
 * Consider for example:
 * int[2][3] a;
 *
 * This is transformed into
 * a = ARRAY[6]; //declare one-dimensional array of length 6
 * MULTIDIM a 0 2 //partition a into 2 dimensions
 * MULTIDIM a 1 3 
 * MULTIARRAY a   //declare it
 *
 * the MULTIDIM instructions will 'declare' the array at run-time
 * (sizes are only known at run time in general), this instruction
 * works closely with the DIMLENGTH instruction.
 */

class MultiDimI: public Instruction {

  public:

    MultiDimI(ArrayRegister* a, NumRegister* nth, VarRegister* dimension):
      array_(a), nth_(nth), dimension_(dimension) { }

    virtual ~MultiDimI();

    /**
     * Returns a printable representation of this instruction
     */
    virtual string toString() const;

    ArrayRegister* getArray() const { return array_; }

    NumRegister* getNthDimension() const { return nth_; }

    VarRegister* getDimLength() const { return dimension_; }

    /**
     * Accepts a CodeVisitor. This method should call back on the visitor
     * itself. It implements a double dispatch.
     */
    virtual void accept(CodeVisitor* v) {
      Instruction::accept(v);
      v->visitMultiDimInstruction(this);
    };

  private:
    ArrayRegister* array_;
    NumRegister* nth_;
    VarRegister* dimension_;
  
};

#endif
