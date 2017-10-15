#ifndef BB_TO_DAG_H
#define BB_TO_DAG_H

#include "../instructions/bbmapping.h"
#include "../instructions/instructions.h"
#include "../operators/operator.h"
#include "dagnode.h"
#include "dagleaf.h"
#include "regcmp.h"

#include <map>
#include <utility>
#include <set>
#include <list>
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

class Register;
class ArrayRegister;
class FunRegister;
class VarRegister;
class NumRegister;
class QCharRegister;

class Instruction;
class Optimizer;

typedef map<Register*,DagNode*,RegCompare> NodeMap;

/**
 * Small structure to store only children of a node
 */
struct Children {
  Children(DagNode* node):
    child1(node->firstChild()),
    child2(node->secondChild()),
    child3(node->thirdChild()) { }
  DagNode* child1;
  DagNode* child2;
  DagNode* child3;
};

typedef list<pair<Children,DagNode*> > ChildList;
typedef map<string, ChildList> DAG; 

/**
 * Used to map an array reference to the node which is the most recently
 * added DAG node for an array assignment instruction (ie a[i]=b).
 * It is necessary to add extra 'constraining' edges in the DAG because
 * references made before the assignment should also be generated before
 * the assignment.
 */

typedef map<ArrayRegister*,DagNode*,RegCompare> ArrayAssignmentMap;

/**
 * Used to map an array reference to the set of nodes representing
 * array references to this array, ie instructions of the form x = a[i].
 * This is used to be able to create extra 'constraining' edges in the
 * DAG for array assignment purposes.
 */

typedef map<ArrayRegister*, DagNodes , RegCompare> ArrayReferencesMap;

/**
 * Used to map a register to the set of DagNodes in the DAG which DEPEND
 * on the value of the register. This map is used to ensure that code for
 * these nodes is generated BEFORE the variable is assigned to.
 *
 * eg. more formally (define in as 'is element of'):
 * Node1 in (dependencyNodes(i)) ^ i in (targets(Node2)) -> Node1.code < Node2.code
 */
typedef map<Register*, set<DagNode*> , RegCompare> DependencyNodes;

/**
 * Used to keep track of the globals this basic block uses or modifies.
 * Necessary because nodes depending on globals should be killed upon
 * execution of a CALL instruction.
 */
typedef set<Register*, RegCompare> RegisterSet;

/**
 * Defines the construction of a DAG from a basic block.
 * The DAG is subject to optimization and is then remapped to a
 * (hopefully) more optimized block.
 */
class BB2DagMapper: public CodeVisitor {

  public:

    /**
     * Map a given basic block for the given function to a DAG and write the
     * optimized version to the given instructionstream.
     */
    BB2DagMapper(BasicBlock& bb, Instructions& optimizedstream, string& funname);

    ~BB2DagMapper();

    /**
     * Construct the dag out of the basic block.
     */
    void makeDAG();

    /**
     * Prints the DAG (as a debugging utility).
     */
    void printDAG();

    /**
     * Maps the DAG back to optimized Three address code
     */
    void optimize();

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

  private:

    /**
     * Returns whether the given node is valid for use by another
     * node as a child. Ie the given node should not be a null-pointer
     * (obviously :) and it should also not have been killed
     */
    bool isValid(DagNode* node) const;

    /**
     * Adds a node to the DAG
     */
    void addToDAG(string& opcode, DagNode* node);

    /**
     * Create a leaf node with the given register
     */
    DagNode* makeLeaf(Register* r);

    /**
     * Create a new node (not a leaf) with the given instruction and children
     */
    DagNode* makeNode(Instruction* i, DagNode* c1, DagNode* c2, DagNode* c3);

    /**
     * Update the dependencies of the current node by removing the old node
     * and adding the new node for each target.
     * forall target in targets(current): dependencies(target) \ { old } U { new }
     */
    void updateDependencies(DagNode* current, DagNode* oldNode, DagNode* newNode);

    /**
     * Adds the given register as a target to the given node. It also
     * adds extra constraining edges to the given node, one to each dependency
     * node of the added register.
     *
     * Formally:
     * node(x) ^ register(r) ^ node(y) ^ isDependency(y,r) ^ isTarget(r,x) -> 
     * (x,y) is an edge in the DAG
     */
    void addTarget(DagNode* node, Register* toAdd);

    /**
     * Match two child nodes connected with the given operator.
     * This matching process also takes into account algebraic identities
     * such as x+0, x*1, x-0, 1*x, x/1 etc.
     * It also takes into account possible commutativity of the operator.
     * @return the matched node or a null pointer if not matched
     */
    DagNode* match(Operator& op, DagNode* child1, DagNode* child2);

    /**
     * Retrieves a node with the given children and the given operator id
     * (acts as a kind of opcode)
     * @return the matched node or a null pointer if not matched
     */
    DagNode* match(string& id, DagNode* child1, DagNode* child2, DagNode* child3);

    /**
     * Looks up a node for the given register. If the node
     * is not found, a null-pointer is returned
     */
    DagNode* lookupNode(Register* r);

    /**
     * Prints a DAG node, helpfunction for printDAG.
     */
    void printNode(DagNode* node,int indent);

    /**
     * Optimizes a DAG node, helpfunction for optimize.
     */
    void optimizeNode(DagNode* node);

    BasicBlock&     block_;
    NodeMap*        nodes_;  /**< holds 'current' value of registers */
    NodeMap*        leaves_; /**< holds 'initial' value of registers */
    DAG*            dag_;
    list<DagNode*>* greatest_; /**< 'greatest' elements of the DAG */

    /** previously generated 'non-assignment' node */
    DagNode*        previousNode_;

    ArrayAssignmentMap* arrayAssign_;
    ArrayReferencesMap* arrayRefs_;

    Optimizer*      optimizer_;

    /** dependenciesOf_[x] represents all Nodes dependent on the value of x */
    DependencyNodes dependenciesOf_;

    /**
     * Set of all global variables used by this basic block.
     *
     * Necessary because when a CALL instruction is executed,
     * all nodes dependent on globals should be killed (because
     * the call may alter the globals and they should thus not
     * be 'shared' among nodes before and after the call)
     */
    RegisterSet     globals_;

};

#endif
