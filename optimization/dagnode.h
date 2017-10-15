#ifndef DAGNODE_H
#define DAGNODE_H

#include "../instructions/instruction.h"
#include "../instructions/reglist.h"
#include "../instructions/varregister.h"

#include <list>

class DagNode;

typedef std::list<DagNode*> DagNodes;

/**
 * A DagNode is a Node in the Directed Acyclic Graph which represents
 * the intermediate code
 */

class DagNode {

  public:

    DagNode(Instruction* original, DagNode* first,
	    DagNode* second, DagNode* third);

    virtual ~DagNode();
    
    /**
     * Acts as a sort of 'OPCODE' for this node.
     */
    Instruction* instruction() const { return instr_; }

    /**
     * Represents the list of all 'targets': all variables
     * assigned to (or modified) by the instruction of this node
     */
    RegisterList& targets() const { return *targets_; }

    /**
     * Returns the preferred target to use by parental nodes, this
     * is the node which most certainly will contain the value of the
     * expression represented by this node.
     * The main purpose of this function is to discard temporaries in
     * favor of normal variables.
     */
    Register* chosenTarget() const { return chosenTarget_; }

    DagNode* firstChild() const { return firstChild_; }

    DagNode* secondChild() const { return secondChild_; }

    DagNode* thirdChild() const { return thirdChild_; }

    /**
     * Returns whether this DAG Node is 'killed' by a previous node, which
     * could be because of array assignment or a function call. When a node
     * is killed, it cannot be 'used' anymore by other nodes/variables.
     */
    bool killed() const { return killed_; }

    /**
     * Returns whether code for this node has already been generated. This
     * is necessary because a node can be 'visited' or 'reached' in different
     * ways. Once code has been generated for the node, we don't have to generate
     * it again when visiting it the next time around. 
     */
    bool generated() const { return generated_; }

    /**
     * Sets the preferred target to be returned by chosenTarget()
     */
    void setChosenTarget(Register* chosen);

    /**
     * Kills off this node, see killed()
     */
    void kill(); 

    /**
     * Marks this node as being generated, see generated()
     */
    void setGenerated(); 

    /**
     * A list of references to other DAG Nodes.
     * These references represent extra 'constraining' edges in the DAG.
     * They ensure that code for those nodes will be generated before
     * my code. They are important for eg Array Assignment and Array References,
     * but also for 'live' variables (they should not be assigned to before
     * they are used)
     */
    DagNodes* references() const { return references_; }

    /**
     * Adds a reference or 'constraining edge' to the given node.
     * Code for this node will thus always be generated before my code.
     */
    void addReference(DagNode* node);

  protected:

    RegisterList* targets_;

  private:
    Instruction* instr_;
    DagNode*     firstChild_;
    DagNode*     secondChild_;
    DagNode*     thirdChild_;

    /** indicates whether this node is killed or not by an AAS */
    bool         killed_;
    /** indicates whether code for this node is already generated */
    bool         generated_;

    Register*    chosenTarget_;
    /** references to update before my code is generated */
    DagNodes*    references_;

};

#endif
