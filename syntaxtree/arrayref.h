#ifndef ARRAY_REF_H
#define ARRAY_REF_H

#include "array.h"
#include "aggregates.h"
#include "../type.h"
#include "../visitors/treevisitor.h"

#include <iostream>
#include <string>

using namespace std;

/**
 * A reference to an array, i.e. a proxy object referring to
 * a 'real' array. This way, the array is never duplicated, only
 * the proxies.
 */

class ArrayRef: public Array {

   public:

      /**
       * Creates a reference to the given array
       */
      ArrayRef(Array* reference): reference_(reference) { }
      
      virtual ~ArrayRef() { }; 

      virtual Type baseType() const;

      virtual Expressions& lengths() const;

      virtual int dimension() const;

      virtual string* name() const;

      virtual void setName(string* newName);

      virtual ostream& prettyPrint(ostream& os=cout,int indent=0) const;

      virtual string toString() const;

      virtual void accept(TreeVisitor* v);

      void setReference(Array* newRef) {
	reference_ = newRef;
      }

      virtual void setIndices(Variables* indexlist);

      virtual Variables* getIndices() const;

      virtual bool isGlobal() const;

   private:

      Array* reference_;

};

#endif
