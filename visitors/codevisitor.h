#ifndef CODEVISITOR_H
#define CODEVISITOR_H

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
 * A CodeVisitor is an abstract superclass of all visitors who
 * can visit Instructions
 *
 * The methods have an empty implementation so that subclasses
 * only have to override the methods they need
 */

class CodeVisitor {

  public:

    CodeVisitor() { }

    virtual ~CodeVisitor() { }

    virtual void visitAddressInstruction(AddressI* addr) { }

    virtual void visitArrayAllocInstruction(ArrayAllocI* aloc) { }

    virtual void visitArrayAccInstruction(ArrayAccessI* aac) { }

    virtual void visitArrayAssInstruction(ArrayAssignmentI* aas) { }

    virtual void visitBinaryAssInstruction(BinAssignmentI* bas) { }

    virtual void visitCallInstruction(CallI* call) { }

    virtual void visitGotoInstruction(GotoI* gotoi) { }

    virtual void visitIfInstruction(IfI* ifi) { }

    virtual void visitLengthInstruction(LengthI* length) { }

    virtual void visitParamInstruction(ParamI* par) { }

    virtual void visitArrayParamInstruction(ArrayParamI* par) { }

    virtual void visitReadInstruction(ReadI* read) { }

    virtual void visitReturnInstruction(ReturnI* ret) { }

    virtual void visitReturnValInstruction(RetValI* rval) { }

    virtual void visitSimpleAssInstruction(SimpleAssignmentI* sim) { }

    virtual void visitUnaryAssInstruction(UnAssignmentI* uas) { }

    virtual void visitWriteInstruction(WriteI* write) { }

    virtual void visitDimLengthI(DimLengthI* dim) { }

    virtual void visitMultiDimInstruction(MultiDimI* multi) { }

    virtual void visitMultiArrayInstruction(MultiArrayI* marr) { }

    virtual void visitArrayRegister(ArrayRegister* areg) { }

    virtual void visitFunRegister(FunRegister* funreg) { }

    virtual void visitVarRegister(VarRegister* varreg) { }

    virtual void visitNumRegister(NumRegister* numreg) { }

    virtual void visitQCharRegister(QCharRegister* creg) { }

    virtual void visitLeader(Instruction* i) { }

};

#endif
