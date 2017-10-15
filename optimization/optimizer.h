#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "dagnode.h"
#include "dagleaf.h"
#include "../visitors/codevisitor.h"
#include "../instructions/reglist.h"
#include "../instructions/instructions.h"

#include <string>

using namespace std;

//forward declarations
class AddressI;
class ArrayAllocI;
class ArrayAccessI;
class ArrayAssignmentI;
class ArrayParamI;
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

/**
 * An Optimizer maps a DAG made from a basic block back to
 * three address code
 */
class Optimizer: public CodeVisitor {

  public:

    /**
     * Optimize the given codestream for the given function.
     */
    Optimizer(Instructions& codestream, string& funname);

    /**
     * Generate code for the given instruction.
     * In case of an assignment instruction, the right register
     * has to be chosen from the targets set. The children list
     * contains the varregisters representing the chosen targets
     * of the children
     */
    void accept(DagNode* node);

    virtual void visitArrayAllocInstruction(ArrayAllocI* aloc);

    virtual void visitArrayAccInstruction(ArrayAccessI* aac);

    virtual void visitArrayAssInstruction(ArrayAssignmentI* aas);

    virtual void visitBinaryAssInstruction(BinAssignmentI* bas);

    virtual void visitCallInstruction(CallI* call);

    virtual void visitGotoInstruction(GotoI* gotoi);

    virtual void visitIfInstruction(IfI* ifi);

    virtual void visitLengthInstruction(LengthI* length);

    virtual void visitParamInstruction(ParamI* par);

    virtual void visitArrayParamInstruction(ArrayParamI* par);

    virtual void visitReadInstruction(ReadI* read);

    virtual void visitReturnInstruction(ReturnI* ret);

    virtual void visitReturnValInstruction(RetValI* rval);

    virtual void visitSimpleAssInstruction(SimpleAssignmentI* sim);

    virtual void visitUnaryAssInstruction(UnAssignmentI* uas);

    virtual void visitWriteInstruction(WriteI* write);

    virtual void visitDimLengthI(DimLengthI* dim);

    virtual void visitMultiDimInstruction(MultiDimI* multi);

    virtual void visitMultiArrayInstruction(MultiArrayI* marr);

    /**
     * Used for cloning an Array Register
     */
    virtual void visitArrayRegister(ArrayRegister* areg);

    /**
     * Used for cloning a Function Register
     */
    virtual void visitFunRegister(FunRegister* funreg);

    /**
     * Used for cloning a Variable Register
     */
    virtual void visitVarRegister(VarRegister* varreg);

    /**
     * Used for cloning a Numeric Register
     */
    virtual void visitNumRegister(NumRegister* numreg);

    /**
     * Used for cloning a Character Register
     */
    virtual void visitQCharRegister(QCharRegister* creg);

    virtual void visitLeader(Instruction* i);

  private:

    /**
     * (presumably) safe downcast to a VarRegister. Presumably safe
     * because it is only called by instructions who require a varregister
     * at a certain position in the instruction
     */
    VarRegister* toVarRegister(Register* reg);

    /**
     * (presumably) safe downcast to an ArrayRegister. Presumably safe
     * because it is only called by instructions who require an ArrayRegister
     * at a certain position in their instruction
     */
    ArrayRegister* toArrayRegister(Register* reg);

    /**
     * (presumably) safe downcast to an NumRegister. Presumably safe
     * because it is only called by instructions who require an NumRegister
     * at a certain position in their instruction
     */
    NumRegister* toNumRegister(Register* reg);

    /**
     * Generates code for a leaf. If the leaf has no targets, the
     * identifier constituting the leaf becomes the chosen target
     * and no code is generated at all. Otherwise, an assignment
     * is generated for each target the leaf has.
     */
    void optimizeLeaf(DagLeaf* leaf);

    /**
     * Target selection strategy.
     * Selects 'real' variables above 'temporary' (ie generated during
     * IC code generation) variables.
     * Saves its result in chosenTarget_.
     * Also saves all variables that need to be 'updated' in the given
     * updatelist. This is necessary in the case of multiple 'live' variables.
     */
    void chooseTarget(RegisterList& targets, RegisterList& updates);

    /**
     * Updates the registers in the updatelist. This means generating
     * assignment statements that copy the value from the source into
     * the register to update.
     */
    void updateTargets(Register* source, RegisterList& updates);

    /**
     * Clones a given register
     */
    Register* clone(Register* reg);

    Instructions& codestream_;
    DagNode*      node_;
    Instruction*  instruction_;
    RegisterList* targets_;
    Register*     childTarget1_;
    Register*     childTarget2_;
    Register*     childTarget3_;
    Register*     chosenTarget_;
    Register*     clone_;
    string&       funName_;
  
};

#endif
