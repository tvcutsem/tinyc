#ifndef TYPECHECK_EXCEPTIONS_H
#define TYPECHECK_EXCEPTIONS_H

#include "../exceptions.h"
#include "../syntaxtree/node.h"
#include "../syntaxtree/returnstmt.h"
#include "../syntaxtree/funcall.h"
#include "../syntaxtree/assign.h"

using namespace std;

/**
 * Common abstract superclass of all errors detected when
 * doing a typecheck
 */

class TypeCheckError: public Exception {

   public:

     /**
      * @param node the abstract syntax tree node where the error occured
      */
     TypeCheckError(Node* node,string errname,string description):
	Exception(description),descr_(description),errname_(errname),node_(node) { }

     virtual ~TypeCheckError();

     /**
      * @return a string description of this kind of error
      */
     virtual string description() const { return descr_; }

     /**
      * @return the name of this kind of error
      */
     virtual string errname() const { return errname_; }

     /**
      * @return the line number where the error occured
      */
     virtual int lineno() const;

     /**
      * @see description()
      */
     string what();

     virtual ostream& report(ostream& errstream=cerr) const;

   private:

     string descr_;
     string errname_;
     Node* node_;

};

/**
 * Thrown when an illegal assignment occurs (incompatible types),
 * or when an illegal type is returned.
 */

class TypeMismatchError: public TypeCheckError {

   public:
     TypeMismatchError(string description,Type expected,Type given,Node* node):
	TypeCheckError(node,"Type Mismatch",description),
     	expected_(expected),given_(given) { }

     virtual string description() const;
   private:
     Type expected_;
     Type given_;
};

/**
 * Thrown when an illegal assignment occurs
 */

class AssignmentError: public TypeMismatchError {
   public:
     AssignmentError(Type expected,Type given,Assignment* node):
	TypeMismatchError("Illegal Types in Assignment",expected,given,node) { }
};

/**
 * Thrown when an incompatible type is returned
 */

class ReturnError: public TypeMismatchError {
   public:
     ReturnError(Type expected,Type given,ReturnStatement* node):
	TypeMismatchError("Type of returned expression does not match returntype",
	      		  expected,given,node) { }
};

/**
 * Thrown when an operator receives an argument of invalid type
 */

class OperatorTypeError: public TypeCheckError {
   public:
     OperatorTypeError(Type given,Node* node):
	TypeCheckError(node,"Type Error","Invalid argument type given to operator"),
     	givenType_(given) { }

     virtual string description() const;
   private:
     Type givenType_;
};

/**
 * Thrown when an invalid number of arguments is passed
 * in a function call
 */

class ArityError: public TypeCheckError {

   public:
     ArityError(int expectedArgs,int givenArgs,Funcall* node):
	TypeCheckError(node,"Arity Error","Invalid number of arguments passed"),
	expectedArgs_(expectedArgs),givenArgs_(givenArgs) { }

     virtual string description() const;

   private:
     int expectedArgs_;
     int givenArgs_;
};

/**
 * Thrown when an invalid index type is given to index an array
 */

class IllegalIndexError: public TypeCheckError {

   public:
     IllegalIndexError(int nth,Type given,Node* node):
	TypeCheckError(node,"Invalid Index","Invalid index type to index an array"),
	nth_(nth),givenType_(given) { }

     virtual string description() const;

   private:
     int nth_;
     Type givenType_;
};

/**
 * Thrown when one accesses an array with an invalid number of indices
 */

class ArrayDimensionError: public TypeCheckError {
   public:
      ArrayDimensionError(int expected,int given,Node* node):
	 TypeCheckError(node,"Dimension Mismatch",
	                     "Invalid dimension specified for array"),
      			     expectedDim_(expected),givenDim_(given) { }

      virtual string description() const;
   private:
      int expectedDim_;
      int givenDim_;
};

/**
 * Thrown when an actual argument does not match it's formal counterpart
 */

class ParameterTypeError: public TypeMismatchError {
   public:
      ParameterTypeError(int nr,Type expected, Type given, Funcall* fun):
	 TypeMismatchError("Mismatched type in passing argument",expected,given,fun),
         nth_(nr) { }

      virtual string description() const;

   private:
      int nth_; /**< it is the n'th argument causing problems */ 

};

/**
 * Thrown when conditions do not evaluate to an integral value
 */

class ConditionError: public TypeCheckError {
   public:
      ConditionError(Type given,Expression* condition):
	 TypeCheckError(condition,"Type Error",
	       "Condition does not evaluate to an integral value"),
         given_(given) { }
      virtual string description() const;
   private:
      Type given_;

};

/**
 * Thrown when a 'read function' or 'write function' is encountered
 */

class UnexpectedFunctionError: public TypeCheckError {
   public:
      UnexpectedFunctionError(Node* node):
	 TypeCheckError(node,"Unexpected function",
	       		     "Invalid use of variable denoting a function") { }
};

/**
 * Thrown when one wants to take the length of something other than an
 * array, or when we are not indexing an array
 */

class ArrayExpectedError: public TypeCheckError {
   public:
      ArrayExpectedError(Node* node):
	 TypeCheckError(node,"Array expected",
	       		     "This operation requires an array reference") { }
};

/**
 * Thrown when one wants to read into an entire array or wants to write
 * out an entire array, or when one wants to assign to an array
 */

class UnexpectedArrayError: public TypeCheckError {
   public:
      UnexpectedArrayError(Node* node):
	 TypeCheckError(node,"Unexpected array",
	       		     "Invalid use of variable denoting an array") { }
};


/*
 * Thrown when an actual argument array has a different base type than
 * its formal counterpart
 */

class ArrayBaseMismatch: public TypeMismatchError {
   public:
      ArrayBaseMismatch(int nr,Type expected, Type given, Funcall* fun):
	 TypeMismatchError(
	     "Mismatched array base types in passing argument",expected,given,fun),
         nth_(nr) { }

      virtual string description() const;

   private:
      int nth_; /**< it is the n'th argument causing problems */ 

};

/*
 * Thrown when an actual argument array has a different dimensionality than
 * its formal counterpart
 */

class DimensionMismatch: public ArrayDimensionError {
   public:
      DimensionMismatch(int nr,int expected, int given, Funcall* fun):
     	ArrayDimensionError(expected,given,fun), nth_(nr) { }

      virtual string description() const;

   private:
      int nth_; /**< it is the n'th argument causing problems */ 

};

#endif
