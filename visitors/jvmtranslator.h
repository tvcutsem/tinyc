#ifndef JVM_TRANSLATOR_H
#define JVM_TRANSLATOR_H

#include "treevisitor.h"
#include "codevisitor.h"
#include "operatorvisitor.h"
#include "../type.h"
#include "../instructions/register.h"
#include "../optimization/regcmp.h"

#include <iostream>
#include <sstream>
#include <string>
#include <set>

using namespace std;

//forward declarations

class Variable;
class Identifier;
class Array;
class Function;

class AddressI;
class ArrayAllocI;
class ArrayAccessI;
class ArrayAssignmentI;
class BinAssignmentI;
class CallI;
class GotoI;
class IfI;
class LengthI;
class ParamI;
class ReadI;
class ReturnI;
class RetValI;
class SimpleAssignmentI;
class UnAssignmentI;
class WriteI;
class DimLengthI;
class MultiDimI;
class MultiArrayI;

class ArrayRegister;
class FunRegister;
class VarRegister;
class NumRegister;
class QCharRegister;

class Instruction;

class DividesOp;
class EqualOp;
class GreaterOp;
class LessOp;
class MinusOp;
class NEqualOp;
class PlusOp;
class TimesOp;
class UMinusOp;
class NegateOp;

typedef set<Register*,RegCompare> RegisterSet;

/**
 * A Java Virtual Machine Translator translates the generic three address
 * code used for optimisation to JVM assembler statements.
 * It does this by visiting a sequence of instructions and mapping each
 * to its corresponding JVM code.
 */

class JVMTranslator: public TreeVisitor, public CodeVisitor, public OperatorVisitor { 

  public:

    JVMTranslator(string& funName, string& className, ostream& codestream);

    ~JVMTranslator();
    
    /*
     * A Variable can be an ordinary or a temporary variable.
     * When using temporaries, these are stored on the stack,
     * rather than loaded and saved. Thus, they do not have to be
     * declared.
     *
     * Some instructions will be type-dependent. The comment tries
     * to be generic.
     */


    /**
     * Visits a declaration of a variable inside the function body
     * (thus, assumed to be a non-global variable)
     * var
     *  ->
     *   type var
     */
    virtual void visitVariable(Variable* var);

    /**
     * Visits a declaration of an array inside the function body
     * (thus, assumed to be a non-global array)
     * array
     *  ->
     *   array.base_type[] array
     */
    virtual void visitArray(Array* arr);

    /**
     * ret_type function(type arg1, ... type argN)
     *  ->
     * method public static ret_type function(type arg1, ..., type argN) max_stack {
     *  parameterdecl's
     *  declarations
     *  store TOS args into parameter vars
     *  body
     * }
     *
     * Note the use of a static (class-) method, since no java object
     * is necessary to execute on
     */
    virtual void visitFunction(Function* fun);

    //code inherited from CodeVisitor
    //map each instruction to a JVM assembler statement

    /**
     * arr = ARRAY var
     *  ->
     * load var
     * newarray arr.base_type
     * store arr
     *
     * for multidimensional arrays, also add:
     * int[] _arr_dimensions_
     * ldc arr.nr_of_dimensions
     * newarray int
     * astore _arr_dimensions_
     */
    virtual void visitArrayAllocInstruction(ArrayAllocI* aloc);

    /**
     * var = arr[idx]
     *  ->
     * aload arr
     * load idx
     * iaload
     * store var
     */
    virtual void visitArrayAccInstruction(ArrayAccessI* aac);

    /**
     * arr[idx] = var
     *  ->
     * aload arr
     * load idx
     * load var
     * iastore
     */
    virtual void visitArrayAssInstruction(ArrayAssignmentI* aas);

    /**
     * var = arg1 op arg2
     *  ->
     * load arg1
     * load arg2
     * translate(op)
     * store var
     */
    virtual void visitBinaryAssInstruction(BinAssignmentI* bas);

    /**
     * CALL f,n
     *  ->
     * invokestatic f.signature
     */
    virtual void visitCallInstruction(CallI* call);

    /**
     * GOTO label
     *  ->
     * goto label
     */
    virtual void visitGotoInstruction(GotoI* gotoi);

    /**
     * IF arg1 op arg2 GOTO label
     *  ->
     * load arg1
     * load arg2
     * translate(op)
     */
    virtual void visitIfInstruction(IfI* ifi);

    /**
     * var = LENGTH arr
     *  ->
     * aload arr
     * arraylength
     * store var
     */
    virtual void visitLengthInstruction(LengthI* length);

    /**
     * PARAM var
     *  ->
     * load var
     */
    virtual void visitParamInstruction(ParamI* par);

    /**
     * ARRAYPARAM arr
     *  ->
     * aload arr
     */
    virtual void visitArrayParamInstruction(ArrayParamI* par);

    /**
     * READ var
     *  ->
     * getstatic java.io.InputStream java.lang.System.in
     * invokevirtual int java.io.InputStream.read()
     * i2c
     * store var
     *
     * (ie var = (char) System.in.read(); )
     *
     * When reading into an integer, we should convert the
     * character to a real integer:
     *
     * invokestatic int java.lang.Character.getNumericValue(char)
     */
    virtual void visitReadInstruction(ReadI* read);

    /**
     * RETURN var
     *  ->
     * load var
     * ireturn
     */
    virtual void visitReturnInstruction(ReturnI* ret);

    /**
     * var = RETVAL
     *  ->
     * store var
     */
    virtual void visitReturnValInstruction(RetValI* rval);

    /**
     * var1 = var2
     *  ->
     * load var2
     * store var1
     */
    virtual void visitSimpleAssInstruction(SimpleAssignmentI* sim);

    /**
     * var1 = op var2
     *  ->
     * load var2
     * translate(op)
     * store var1
     */
    virtual void visitUnaryAssInstruction(UnAssignmentI* uas);

    /**
     * WRITE var
     *  ->
     * getstatic java.io.PrintStream java.lang.System.out
     * load var
     * invokevirtual void java.io.PrintStream.print(int)
     *
     * (ie System.out.print(var); )
     */
    virtual void visitWriteInstruction(WriteI* write);

    /**
     * var = DIMLENGTH a, i
     *  ->
     * aload a
     * ldc i
     * invokestatic int TinyRuntime.dimension(Object,int)
     * istore var
     *
     * The tiny runtime environment is used to retrieve the dimension
     * of a previously allocated multi-dimensional array.
     */
    virtual void visitDimLengthI(DimLengthI* dim);

    /**
     * MULTIDIM a n x
     *  ->
     * aload _a_dimensions_
     * ldc n
     * iload x
     * iastore
     */
    virtual void visitMultiDimInstruction(MultiDimI* multi);

    /**
     * MULTIARRAY a
     *  ->
     * aload a
     * aload _a_dimensions_
     * invokestatic void TinyRuntime.declareArray(Object, int[])
     */
    virtual void visitMultiArrayInstruction(MultiArrayI* marr);

    /**
     * If the instruction is a leader, generate a JVM label
     * so that goto's can jump to here
     *  ->
     * label:
     */
    virtual void visitLeader(Instruction* i);

    //methods inherited from OperatorVisitor
    //these generate code for the operators themselves

    /**
     * var / var
     *  ->
     * idiv
     */
    virtual void visitDividesOp(DividesOp* op);
    
    /**
     * var == var
     *  ->
     * if_icmpeq
     */
    virtual void visitEqualOp(EqualOp* op);

    /**
     * var > var
     *  ->
     * if_icmpgt
     */
    virtual void visitGreaterOp(GreaterOp* op);

    /**
     * var < var
     *  ->
     * if_icmplt
     */
    virtual void visitLessOp(LessOp* op);

    /**
     * var - var
     *  ->
     * isub
     */
    virtual void visitMinusOp(MinusOp* op);

    /**
     * var != var
     *  ->
     * if_icmpne
     */
    virtual void visitNEqualOp(NEqualOp* op);

    /**
     * var + var
     *  ->
     * iadd 
     */
    virtual void visitPlusOp(PlusOp* op);

    /**
     * var * var
     *  ->
     * imul 
     */
    virtual void visitTimesOp(TimesOp* op);

    /**
     * - var
     *  ->
     * ineg 
     */
    virtual void visitUMinusOp(UMinusOp* op);

  private:

    //nested classes
    
    /**
     * class used to generate code for saving registers
     * Temporaries are not saved, because it is expected
     * their value can remain on the stack.
     */
    class RegisterSaver: public CodeVisitor {

      public:

	RegisterSaver(ostream& codestream, JVMTranslator& translator):
	  codestream_(codestream), outer(translator) { }

	/**
	 * astore arr
	 */   
	virtual void visitArrayRegister(ArrayRegister* areg);

	/**
	 * store var
	 */   
	virtual void visitVarRegister(VarRegister* varreg);

      private:
	ostream& codestream_;
	JVMTranslator& outer;

    };

    /**
     * class used to generate code for loading registers
     * Temporaries are not loaded, they are expected
     * to be on top of stack.
     */
    class RegisterLoader: public CodeVisitor {

      public:

	RegisterLoader(ostream& codestream, JVMTranslator& translator):
	  codestream_(codestream), outer(translator) { }

	/**
	 * aload arr
	 */   
	virtual void visitArrayRegister(ArrayRegister* areg);

	/**
	 * load var
	 */   
	virtual void visitVarRegister(VarRegister* varreg);

	/**
	 * if (-128 <= num <= 127)
	 *    //JVM machine specific one-byte signed integer push (more efficient)
	 *    bipush num
	 * else
	 *    ldc num
	 *
	 * Moreover, the constants 0, 1, 2, 3, 4 and 5 can be represented by
	 * iconst_0, iconst_1, iconst_2, iconst_3, iconst_4 and iconst_5
	 * -1 is represented by iconst_m1
	 * These instructions are more efficient and take up less space
	 * in the generated bytecode
	 */   
	virtual void visitNumRegister(NumRegister* numreg);

	/**
	 * ldc char
	 */   
	virtual void visitQCharRegister(QCharRegister* creg);

      private:
	ostream& codestream_;
	JVMTranslator& outer;

    };

    friend class RegisterSaver;
    friend class RegisterLoader;

    /**
     * Convencience function for saving registers
     */
    void save(Register* r);

    /**
     * Convencience function for loading registers
     */
    void load(Register* r);


    /**
     * Generates code for a function parameter declaration.
     */
    string generateDeclaration(Identifier* id, stringstream& codestream, int index);

    /**
     * Generates code for all used temporary variables
     */
    void generateTempDeclarations(RegisterSet& usedTemps,
				  ostringstream& declarationstream);

    /**
     * Checks whether the binary assignment is of the form
     *  x = x + C or
     *  x = C + x or
     *  x = x - C
     * with x a non-temporary variable and C in [-128,127].
     * If this is the case, much more efficient code is generated than usual
     * @return true if optimization succeeded, false otherwise
     */
    bool checkForIncrement(BinAssignmentI* bas);

    /**
     * Generates efficient code for:
     *  x = x + C if neg is false
     *  x = x - C if neg is true
     *  @return true if succeeded, false is C not in [-128,127]
     */
    bool generateIncCode(VarRegister* target, VarRegister* value, bool neg);

    /**
     * For a given array a, returns
     *  _a_dimensions_
     */  
    string getArrayDimensions(ArrayRegister* array);

    /**
     * Increment current stack size
     */
    void incStack(int cnt=1);

    /**
     * Decrement current stack size
     */
    void decStack(int cnt=1);

    /**
     * Return the TOS and set it to 0 so that it will not be
     * saved on subsequent 'loads'
     */
    //Register* getTos();

    /**
     * Returns whether the given register is holding a value which is
     * currently on top of stack.
     * @param reg is this register on top of stack?
     */
    bool onTop(Register* reg);

    //instance variables
    //
    /** name of the function being processed */ 
    string funName_;

    /** name of the class file to which this code is written */
    string className_;

    /** output stream to which the code is written */
    ostream& outstream_;

    /** visitor to generate register saves */
    RegisterSaver* registerSaver_;

    /** visitor to generate register loads */
    RegisterLoader* registerLoader_;

    /** the 'top of stack' variable */
    Register* tos_;

    /** maximum stack size at any given time during execution */
    int maxStack_;

    /** output stream for the body itself */
    ostringstream codestream_;

    /** current stack size during code generation, used to calculate maxStack */
    int currentStack_;

    /** set of all used temporary variables, needed to declare them */
    RegisterSet usedTemps_;

};

#endif
