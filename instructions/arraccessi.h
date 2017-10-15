#ifndef ARR_ACCESS_I_H
#define ARR_ACCESS_I_H

#include "assigni.h"
#include "varregister.h"
#include "arrayregister.h"
#include "../syntaxtree/array.h"
#include "../visitors/codevisitor.h"

#include <string>

using namespace std;

/**
 * An instruction representing
 *  x = a[idx]
 */
class ArrayAccessI: public AssignmentI {

  public:
    ArrayAccessI(VarRegister* target,ArrayRegister* array,VarRegister* index);

    ~ArrayAccessI();

    //accessors
    ArrayRegister* getArrayRegister() const { return array_; }
    VarRegister* getIndex() const { return index_; }
    
    //mutators
    void setArrayRegister(ArrayRegister* array) {
      delete array_;
      array_ = array;
    }

    void setIndex(VarRegister* index) {
      delete index_;
      index_ = index;
    }

    string toString() const;

    void accept(CodeVisitor* v) {
      Instruction::accept(v);
      v->visitArrayAccInstruction(this);
    }

  private:
    ArrayRegister* array_;
    VarRegister* index_;
  
};

#endif
