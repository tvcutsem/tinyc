#ifndef ARRAY_DECL_H
#define ARRAY_DECL_H

#include "array.h"
#include "aggregates.h"
#include "../type.h"
#include "../visitors/treevisitor.h"

#include <iostream>
#include <string>

using namespace std;

/**
 * An array declaration which consists of a base type and a
 * number of length expressions (ie arbitrary dimensional arrays)
 */
class ArrayDec: public Array {

   public:

      /**
       * @param base the base type of this array
       * @param lengthExps a sequence of expressions denoting the length of
       * each dimension of the array
       */
      ArrayDec(string* name,Type base,Expressions* lengthExps, bool isGlobal);
      
      virtual ~ArrayDec(); 

      virtual Type baseType() const { return baseType_; }

      virtual Expressions& lengths() const { return *lengthExps_; }

      virtual int dimension() const;

      virtual string* name() const { return name_; }

      virtual void setName(string* newName) { name_ = newName; } 

      virtual ostream& prettyPrint(ostream& os=cout,int indent=0) const;

      virtual string toString() const;

      virtual void accept(TreeVisitor* v);

      virtual void setIndices(Variables* indexlist);

      virtual Variables* getIndices() const;

      virtual bool isGlobal() const { return isGlobal_; }

   private:

      string* name_;
      Type baseType_;
      bool isGlobal_;
      Expressions* lengthExps_;

      Variables* indexRegisters_;

};

#endif
