#ifndef DIM_LEN_I_H
#define DIM_LEN_I_H

#include "assigni.h"
#include "arrayregister.h"
#include "varregister.h"
#include "numregister.h"
#include "../visitors/codevisitor.h"

/**
 * A DIMLENGTH instruction takes an array and an integer, and
 * returns the length of the dimension of that array.
 *
 * This is necessary, imagine the following code excerpt:
 *
 * void f() {
 *   int[2][3] a;
 *   f2(a);
 * }
 *
 * void f2(int[0][0] b) {
 *   ...
 * }
 *
 * How is f2 to know what dimensions b has? They are certainly not 0.
 * The problem here is that it is important for us to have the lengths of
 * the individual arrays, not only the size of the entire array (which is 6).
 * These sizes play a critical role as the offset for an expression like:
 *  b[1][2], which should be translated to:
 *  
 *  b[1*3+2], where 3 is the array's second dimension
 *
 * Therefore, the following code is generated:
 *
 * for int[2][3] a:
 *  ARRAY a 6      //instantiate array of length 6
 *  MULTIDIM a 0 2 //interpret a as a multidimensional array of 2 by 3
 *  MULTIDIM a 1 3 
 *  MULTIARRAY a   //declare the multidimensional array
 *
 * for int[0][0] b:
 *  t0 = DIMLENGTH a 0  //store length of dimension 1 in t0
 *  t1 = DIMLENGTH a 1
 *
 * for x = b[1][2]:
 *  t2 = 1 * t0  //can be optimized
 *  t2 = t2 + 3
 *  x  = b[t2]
 */

class DimLengthI: public AssignmentI {

  public:
    DimLengthI(VarRegister* target,ArrayRegister* array,NumRegister* dimension):
      AssignmentI(target), array_(array), dim_(dimension) { }
    
    virtual ~DimLengthI();

    virtual string toString() const;

    ArrayRegister* getArray() const { return array_; }

    NumRegister* getDimension() const { return dim_; }

    virtual void accept(CodeVisitor* v) {
      Instruction::accept(v);
      v->visitDimLengthI(this);
    };

  private:
    ArrayRegister* array_;
    NumRegister* dim_;
  
};

#endif
