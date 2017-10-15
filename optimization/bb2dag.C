#include "bb2dag.h"
#include "matcher.h"

//Instruction classes
#include "../instructions/instruction.h"
#include "../instructions/addressi.h"
#include "../instructions/arralloci.h"
#include "../instructions/arrparami.h"
#include "../instructions/arraccessi.h"
#include "../instructions/arrassigni.h"
#include "../instructions/binassigni.h"
#include "../instructions/calli.h"
#include "../instructions/gotoi.h"
#include "../instructions/ifi.h"
#include "../instructions/lengthi.h"
#include "../instructions/parami.h"
#include "../instructions/readi.h"
#include "../instructions/returni.h"
#include "../instructions/retvali.h"
#include "../instructions/simpleassigni.h"
#include "../instructions/unassigni.h"
#include "../instructions/writei.h"
#include "../instructions/dimlengthi.h"
#include "../instructions/multidimi.h"
#include "../instructions/multiarrayi.h"

#include "../instructions/varregister.h"
#include "../instructions/funregister.h"
#include "../instructions/arrayregister.h"
#include "../instructions/numregister.h"
#include "../instructions/charregister.h"
#include "../instructions/tempregister.h"

//Operator classes
#include "../operators/divides.h"
#include "../operators/equal.h"
#include "../operators/evaluate.h"
#include "../operators/greater.h"
#include "../operators/less.h"
#include "../operators/minus.h"
#include "../operators/negate.h"
#include "../operators/nequal.h"
#include "../operators/plus.h"
#include "../operators/times.h"
#include "../operators/uminus.h"
#include "../operators/operators.h"

#include "optimizer.h"

#include <iostream>

BB2DagMapper::BB2DagMapper(BasicBlock& bb,
                           Instructions& optimizestream,
			   string& funname):
   block_       (bb),
   nodes_       (new NodeMap),
   leaves_      (new NodeMap),
   dag_         (new DAG),
   greatest_    (new DagNodes),
   previousNode_(0),
   arrayAssign_ (new ArrayAssignmentMap),
   arrayRefs_   (new ArrayReferencesMap),
   optimizer_   (new Optimizer(optimizestream,funname)) { }

BB2DagMapper::~BB2DagMapper() {

  //delete all leaves
  for (NodeMap::iterator n=leaves_->begin(); n!= leaves_->end(); n++) {
    delete n->second;
  }

  //delete the DagNodes from the dag
  for (DAG::iterator nref = dag_->begin(); nref != dag_->end(); nref++) {
    // *nref is a pair<string,list<pair<Children, DagNode*> > >
    ChildList& childs(nref->second);
    for (ChildList::iterator p = childs.begin(); p!= childs.end(); p++) {
      // *p is a pair<Children,DagNode*>
      delete p->second;
    }
  }

  delete nodes_;
  delete leaves_;
  delete dag_;
  delete greatest_;
  delete arrayAssign_;
  delete arrayRefs_;
  delete optimizer_;
}

void BB2DagMapper::makeDAG() {
  for (bbiterator i = block_.begin(); i!=block_.end(); i++)
    // *i is an Instruction*
    (*i)->accept(this);
}

void BB2DagMapper::visitArrayAllocInstruction(ArrayAllocI* aloc) {
  //a = array(length)

  //retrieve the child nodes (or create new ones)
  DagNode* arraynode = lookupNode(aloc->getTarget());
  if (!isValid(arraynode))
    arraynode = makeLeaf(aloc->getTarget());
  DagNode* lengthnode = lookupNode(aloc->getLength());
  if (!isValid(lengthnode))
    lengthnode = makeLeaf(aloc->getLength());
  
  //add the new node
  string opcode("arrayalloc");
  DagNode* allocNode = makeNode(aloc,previousNode_,arraynode,lengthnode);
  addToDAG(opcode,allocNode);

  (*nodes_)[aloc->getTarget()] = allocNode;

  //set myself as the new previousNode_
  previousNode_ = allocNode;
}

void BB2DagMapper::visitArrayAccInstruction(ArrayAccessI* aac) {
  //x = a[i]
  //find the children (a and i)
  DagNode* arraynode = lookupNode(aac->getArrayRegister());
  if (!isValid(arraynode))
    arraynode = makeLeaf(aac->getArrayRegister());
  DagNode* indexnode = lookupNode(aac->getIndex());
  if (!isValid(indexnode))
    indexnode = makeLeaf(aac->getIndex());

  //manage array assignment problems
  //if there was an assignment before this,
  //add that assignment as a child of mine (ie it must be generated before me)
  DagNode* assignNode(0);
  ArrayAssignmentMap::iterator assgn(arrayAssign_->find(aac->getArrayRegister()));
  if (assgn != arrayAssign_->end()) {
    //add assign(a) as a child of x = a[i]
    assignNode = assgn->second;
  }

  //create the new node
  string opcode("arrayaccess");
  DagNode* aacNode = match(opcode,assignNode,arraynode,indexnode);
  //because the assignNode is also used in the matching process,
  //an already 'killed' node will not be matched (because it will
  //have another assignNode than this node
  if (!isValid(aacNode)) {
    //if the node is killed, we'll have to create a new one
    aacNode = makeNode(aac,assignNode,arraynode,indexnode); 
    addToDAG(opcode,aacNode);

    //add myself to the list of made references to the given array
    //(a new array assignment will then make sure I am generated before it)
    (*arrayRefs_)[aac->getArrayRegister()].push_back(aacNode);
  }

  //bookkeeping for the target
  (*nodes_)[aac->getTarget()] = aacNode;
  addTarget(aacNode, aac->getTarget());
}


void BB2DagMapper::visitArrayAssInstruction(ArrayAssignmentI* aas) {
  //a[j] = y
  //find the three child nodes a, j and y
  DagNode* arraynode = lookupNode(aas->getArrayRegister());
  if (!isValid(arraynode))
    arraynode = makeLeaf(aas->getArrayRegister());
  DagNode* indexnode = lookupNode(aas->getIndex());
  if (!isValid(indexnode))
    indexnode = makeLeaf(aas->getIndex());
  DagNode* valuenode = lookupNode(aas->getValue());
  if (!isValid(valuenode))
    valuenode = makeLeaf(aas->getValue());

  string opcode("arrayassignment");
  DagNode* aasNode = makeNode(aas,arraynode,indexnode,valuenode); 
  addToDAG(opcode,aasNode);

  //for each x=a[i] node in the references to a, add this node
  //to my reflist, because these references absolutely have to
  //be generated before myself, also these nodes are killed!
  DagNodes& references = (*arrayRefs_)[aas->getArrayRegister()];
  for (DagNodes::iterator ref=references.begin(); ref != references.end(); ref++) {
    // *ref is a DagNode*, denoting an instruction x=a[i]
    aasNode->addReference(*ref);
    //remove those nodes as greatest elements, they are pointed at by me
    greatest_->remove(*ref);
    //kill it
    (*ref)->kill();
  }
  //clear the refs list: the current refs are added
  references.clear();

  //set myself as the new, current assignment (used by arrayaccess)
  (*arrayAssign_)[aas->getArrayRegister()] = aasNode;
}


void BB2DagMapper::visitBinaryAssInstruction(BinAssignmentI* bas) {
  //x = y op z

  //retrieve both child nodes (or create new ones)
  DagNode* ynode = lookupNode(bas->getFirstArg());
  if (!isValid(ynode))
    ynode = makeLeaf(bas->getFirstArg());
  DagNode* znode = lookupNode(bas->getSecondArg());
  if (!isValid(znode))
    znode = makeLeaf(bas->getSecondArg());
  
  //look for an already existing 'y op z' node (with algebraic matching)
  DagNode* existingNode = match(*bas->getOperator(),ynode,znode);
  if (!isValid(existingNode)) {
    string opcode(bas->getOperator()->symbol());
    existingNode = makeNode(bas,ynode,znode,0);
    addToDAG(opcode,existingNode);
  }
  (*nodes_)[bas->getTarget()] = existingNode;
  addTarget(existingNode, bas->getTarget());
}

void BB2DagMapper::visitCallInstruction(CallI* call) {
  //call f
  //add f as a new node which follows the previously
  //generated non-assignment node
  DagNode* callNode = makeNode(call,previousNode_,0,0);
  //add f to the dag
  string opcode("call");
  addToDAG(opcode,callNode);

  //set myself as the new previousNode_
  previousNode_ = callNode;

  //now, kill all nodes in the DAG which depend upon (or modify)
  //a global variable. This is necessary since this call can
  //arbitrarily modify globals and their nodes should thus not be
  //'shared' or 'reused' after the CALL
  for (RegisterSet::iterator g = globals_.begin(); g != globals_.end(); g++) {
    // *g is a Register*
    set<DagNode*>& globalDeps(dependenciesOf_[*g]);
    typedef set<DagNode*>::iterator Nodeit;
    for (Nodeit node = globalDeps.begin(); node != globalDeps.end(); node++) {
      (*node)->kill();
    }
    //also, kill the current node of the global
    (*nodes_)[*g]->kill();
  }
}

void BB2DagMapper::visitGotoInstruction(GotoI* gotoi) {
  //last statement in this block, thus, because of the structure
  //of previousnodes a greatest element of this DAG
  //just add the GOTO statement as a new structural node
  DagNode* gotoNode = makeNode(gotoi,previousNode_,0,0);

  //add it to the dag
  string opcode("goto");
  addToDAG(opcode,gotoNode);

  //setting myself as previousNode is unnecessary (i'm last)
}

void BB2DagMapper::visitIfInstruction(IfI* ifi) {
  //IF x op y GOTO L
  //retrieve both child nodes (or create new ones)
  DagNode* xnode = lookupNode(ifi->getFirstArg());
  if (!isValid(xnode))
    xnode = makeLeaf(ifi->getFirstArg());
  DagNode* ynode = lookupNode(ifi->getSecondArg());
  if (!isValid(ynode))
    ynode = makeLeaf(ifi->getSecondArg());

  //last statement in this block, thus, because of the structure
  //of previousnodes a greatest element of this DAG
  DagNode* ifNode = makeNode(ifi,previousNode_,xnode,ynode);

  //add it to the dag
  string opcode("if");
  addToDAG(opcode,ifNode);

  //setting myself as previousNode is unnecessary (i'm last)
}
  
void BB2DagMapper::visitLengthInstruction(LengthI* length) {
  //x = length(a)

  //retrieve the array node (or create a new one)
  DagNode* arraynode = lookupNode(length->getSource());
  if (!isValid(arraynode))
    arraynode = makeLeaf(length->getSource());
  
  //first check whether length(a) has already been calculated
  string opcode("length");
  DagNode* lengthNode = match(opcode,arraynode,0,0);
  if (!isValid(lengthNode)) {
    //add the new node
    lengthNode = makeNode(length,arraynode,0,0);
    addToDAG(opcode,lengthNode);
  }

  //bookkeeping for the target variable
  (*nodes_)[length->getTarget()] = lengthNode;
  addTarget(lengthNode, length->getTarget());
}

void BB2DagMapper::visitParamInstruction(ParamI* par) {
  //param x
  //lookup the variable
  DagNode* varnode = lookupNode(par->getValue());
  if (!isValid(varnode))
    varnode = makeLeaf(par->getValue());

  //add param as a new node which follows the previously
  //generated non-assignment node
  DagNode* paramNode = makeNode(par,previousNode_,varnode,0);

  //add the node to the dag
  string opcode("param");
  addToDAG(opcode,paramNode);

  //set myself as the new previousNode_
  previousNode_ = paramNode;
}

void BB2DagMapper::visitArrayParamInstruction(ArrayParamI* par) {
  //arrayparam a
  //lookup the array
  DagNode* arraynode = lookupNode(par->getValue());
  if (!isValid(arraynode))
    arraynode = makeLeaf(par->getValue());

  //add arrayparam as a new node which follows the previously
  //generated non-assignment node
  DagNode* aparamNode = makeNode(par,previousNode_,arraynode,0);

  //add the node to the dag
  string opcode("arrayparam");
  addToDAG(opcode,aparamNode);

  //set myself as the new previousNode_
  previousNode_ = aparamNode;
}

void BB2DagMapper::visitReadInstruction(ReadI* read) {
  //read x
  //lookup the variable
  DagNode* varnode = lookupNode(read->getSource());
  if (!isValid(varnode))
    varnode = makeLeaf(read->getSource());

  //add read as a new node which follows the previously
  //generated non-assignment node
  DagNode* readNode = makeNode(read,previousNode_,varnode,0);

  //add the node to the dag
  string opcode("read");
  addToDAG(opcode,readNode);

  //set myself as the new previousNode_
  previousNode_ = readNode;

  //the 'current value' for x has changed!
  (*nodes_)[read->getSource()] = readNode;
}

void BB2DagMapper::visitReturnInstruction(ReturnI* ret) {
  //return x
  //lookup the variable containing the value to return
  DagNode* varnode = lookupNode(ret->getValue());
  if (!isValid(varnode))
    varnode = makeLeaf(ret->getValue());

  //add ret as a new node which follows the previously
  //generated non-assignment node
  DagNode* retNode = makeNode(ret,previousNode_,varnode,0);

  //add the node to the dag
  string opcode("return");
  addToDAG(opcode,retNode);

  //set myself as the new previousNode_
  previousNode_ = retNode;
}

void BB2DagMapper::visitReturnValInstruction(RetValI* rval) {
  //x = RETVAL

  //just add this node to the DAG for structural information,
  //it does not apply for optimization

  //add rval as a new node which follows the previously
  //generated non-assignment node
  DagNode* rvalNode = makeNode(rval,previousNode_,0,0);

  //add the node to the dag
  string opcode("retval");
  addToDAG(opcode,rvalNode);

  //set myself as the new previousNode_
  previousNode_ = rvalNode;

  //bookkeeping for the target variable, the 'current value' for x changed
  (*nodes_)[rval->getTarget()] = rvalNode;
  addTarget(rvalNode, rval->getTarget());
}

void BB2DagMapper::visitSimpleAssInstruction(SimpleAssignmentI* sim) {
  //x = y
  DagNode* ynode = lookupNode(sim->getSource());
  if (!isValid(ynode))
    ynode = makeLeaf(sim->getSource());

  addTarget(ynode, sim->getTarget());
  (*nodes_)[sim->getTarget()] = ynode;
}

void BB2DagMapper::visitUnaryAssInstruction(UnAssignmentI* uas) {
  //x = op y

  //retrieve the child node (or create a new one)
  DagNode* ynode = lookupNode(uas->getArg());
  if (!isValid(ynode))
    ynode = makeLeaf(uas->getArg());
  
  //look for an already existing 'op y' node
  string id(uas->getOperator()->symbol());
  DagNode* existingNode = match(id,ynode,0,0);
  if (!isValid(existingNode)) {
    existingNode = makeNode(uas,ynode,0,0);
    addToDAG(id,existingNode);
  }
  (*nodes_)[uas->getTarget()] = existingNode;
  addTarget(existingNode, uas->getTarget());
}

void BB2DagMapper::visitWriteInstruction(WriteI* write) {
  //write x
  //lookup the variable
  DagNode* varnode = lookupNode(write->getSource());
  if (!isValid(varnode))
    varnode = makeLeaf(write->getSource());

  //add write as a new node which follows the previously
  //generated non-assignment node
  DagNode* writeNode = makeNode(write,previousNode_,varnode,0);

  //add the node to the dag
  string opcode("write");
  addToDAG(opcode,writeNode);

  //set myself as the new previousNode_
  previousNode_ = writeNode;
}

void BB2DagMapper::visitDimLengthI(DimLengthI* dim) {
  //x = DIMLENGTH a, i

  //just add this node to the DAG for structural information,
  //it does not apply for optimization

  //add dim as a new node which follows the previously
  //generated non-assignment node
  DagNode* dimlengthnode = makeNode(dim,previousNode_,0,0);

  //add the node to the dag
  string opcode("dimlength");
  addToDAG(opcode,dimlengthnode);

  //set myself as the new previousNode_
  previousNode_ = dimlengthnode;

  //bookkeeping for the target variable, the 'current value' for x changed
  (*nodes_)[dim->getTarget()] = dimlengthnode;
  addTarget(dimlengthnode, dim->getTarget());
}


void BB2DagMapper::visitMultiDimInstruction(MultiDimI* multi) {
  //MULTIDIM a, dim

  //retrieve the child nodes (or create new ones)
  DagNode* dimlengthnode = lookupNode(multi->getDimLength());
  if (!isValid(dimlengthnode))
    dimlengthnode = makeLeaf(multi->getDimLength());
  
  //add the new node
  string opcode("multidim");
  DagNode* multidimnode = makeNode(multi,previousNode_,dimlengthnode, 0);
  addToDAG(opcode,multidimnode);

  //set myself as the new previousNode_
  previousNode_ = multidimnode;
}


void BB2DagMapper::visitMultiArrayInstruction(MultiArrayI* marr) {
  //MULTIARRAY a

  //add the new node
  string opcode("multiarray");
  DagNode* multiarraynode = makeNode(marr,previousNode_,0,0);
  addToDAG(opcode,multiarraynode);

  //set myself as the new previousNode_
  previousNode_ = multiarraynode;
}


bool BB2DagMapper::isValid(DagNode* node) const {
  if (node)
    return !(node->killed());
  else
    return 0;
}


void BB2DagMapper::addToDAG(string& opcode, DagNode* node) {
  //insert the node's children into the DAG
  
  //the push_front is important: newer versions of a node (eg in the case
  //of new nodes created after the old ones got killed) will be in front
  //and thus found first (before the killed version) by the match algorithm
  (*dag_)[opcode].push_front(pair<Children,DagNode*>(Children(node),node));

  //remove the new node's children from the greatest_ list:
  //they now have a parent (the new node) and thus cannot be greatest
  if (node->firstChild())
    greatest_->remove(node->firstChild());
  if (node->secondChild())
    greatest_->remove(node->secondChild());
  if (node->thirdChild())
    greatest_->remove(node->thirdChild());

  //add the new node as a greatest node in the DAG
  greatest_->push_back(node);
}

DagNode* BB2DagMapper::makeLeaf(Register* r) {
  DagNode* node = new DagLeaf(r);
  (*leaves_)[r] = node;
  (*nodes_)[r] = node;
  greatest_->push_back(node);

  //if the variable contained in the register is global, add it to
  //the set of globals used or altered by this basic block
  if (r->isGlobal())
    globals_.insert(r);
  
  return node;
}

DagNode* BB2DagMapper::makeNode(Instruction* i,
    				DagNode* c1, DagNode* c2, DagNode* c3) {
  //first, create the new node
  DagNode* node = new DagNode(i,c1,c2,c3);
  //then, announce that this node depends on all targets of its children
  if (c1)
    updateDependencies(c1,c1,node);
  if (c2)
    updateDependencies(c2,c2,node);
  if (c3)
    updateDependencies(c3,c3,node);
  return node;
}

void BB2DagMapper::updateDependencies(DagNode* current,
    				      DagNode* oldNode, DagNode* newNode) {
  RegisterList& targets(current->targets());
  //for each target -> dependencies(target) \ { old } U { new }
  for (Registerit target = targets.begin(); target != targets.end(); target++) {
    set<DagNode*>& deps = dependenciesOf_[*target];
    deps.erase(oldNode);
    deps.insert(newNode);
  }

  DagLeaf* leaf = dynamic_cast<DagLeaf*>(current);
  if (leaf) {
    Register* id(leaf->identifier());
    if (!(id->isConstant())) {
      //only add non-constants (ie variables)
      dependenciesOf_[id].erase(oldNode);
      dependenciesOf_[id].insert(newNode);
    }
    return;
  }
  
  //recursively update the child dependencies
  if (current->firstChild())
    updateDependencies(current->firstChild(),oldNode,newNode);
  if (current->secondChild())
    updateDependencies(current->secondChild(),oldNode,newNode);
  if (current->thirdChild())
    updateDependencies(current->thirdChild(),oldNode,newNode);
}

void BB2DagMapper::addTarget(DagNode* node, Register* toAdd) {
  //add the target to the targetlist of the node
  node->targets().push_back(toAdd);

  //if the variable contained in the register is global, add it to the
  //globals set
  if (toAdd->isGlobal())
    globals_.insert(toAdd);

  //don't add extra edges if the node is a leaf
  DagLeaf* leaf = dynamic_cast<DagLeaf*>(node);
  if (leaf)
    return;
  
  //now, add extra 'constraining edges' to the node: one edge to each
  //node which is a dependency of toAdd.
  set<DagNode*>& deps = dependenciesOf_[toAdd];
  for (set<DagNode*>::iterator dep = deps.begin(); dep != deps.end(); dep++) {
    if (node == *dep)
      continue;
    node->addReference(*dep);
  }
}

DagNode* BB2DagMapper::match(Operator& op, DagNode* child1, DagNode* child2) {
  //if c1 is a leaf, check whether it's argument is neutral according to op
  Matcher isNeutralTo;
  DagLeaf* leaf;
  leaf = dynamic_cast<DagLeaf*>(child1);
  //match(firstArg,op,reg) returns true if reg contains the neutral elt. of op
  if (leaf && isNeutralTo(true,op,leaf->identifier()))
    return child2; //eg z = 1*x or z = 0+x but not z = 0-x
  leaf = dynamic_cast<DagLeaf*>(child2);
  if (leaf && isNeutralTo(false,op,leaf->identifier()))
    return child1; //eg z = x*1 or z = x+0 or z = x-0

  //match x op y
  string opsymbol(op.symbol());
  DagNode* matched = match(opsymbol,child1,child2,0);
  //if not matched, try y op x if op is commutative
  if ((!isValid(matched)) && op.isCommutative())
    return match(opsymbol,child2,child1,0);
  else
    return matched;
}

DagNode* BB2DagMapper::match(string& opcode, DagNode* child1,
    			     DagNode* child2, DagNode* child3) {
  //first, find the ChildList for the given opcode
  DAG::iterator it(dag_->find(opcode)); 
  //now, look for a node with the given children
  if (it != dag_->end()) {
    ChildList& childlist(it->second);
    for (ChildList::iterator p =childlist.begin(); p != childlist.end(); p++) {
      // *p is a pair<Children,DagNode*>
      Children children(p->first);
      if (children.child1==child1 &&
	  children.child2==child2 &&
	  children.child3==child3) {
 	  //childs matched: we have found a node which has these children
	  //and the same opcode
      	  return p->second;
      }
    }
  }
  return 0;
}

DagNode* BB2DagMapper::lookupNode(Register* r) {
  NodeMap::iterator foundNode(nodes_->find(r));
  if (foundNode != nodes_->end())
    return foundNode->second;
  else 
    return 0;
}

void BB2DagMapper::printDAG() {
  //print all greatest elements
  cerr << "basic block -> DAG: " << endl;
  for (DagNodes::iterator n=greatest_->begin(); n != greatest_->end(); n++) {
    printNode(*n,0);
  }
  cerr << endl;
}

void BB2DagMapper::printNode(DagNode* node,int indent) {
  int indentcpy(indent);
  while (indentcpy) {
    cerr << "  ";
    indentcpy--;
  }
  if (node->instruction())
    cerr << node->instruction()->toString();
  else
    cerr << "leaf";
  cerr << " targets[";
  RegisterList& regs(node->targets());
  for (Registerit r = regs.begin(); r != regs.end(); r++) {
    cerr << *((*r)->name()) << ",";
  }
  cerr << "]" << endl;
  //print out all the references first
  DagNodes* refs = node->references();
  for (DagNodes::iterator ref=refs->begin(); ref!=refs->end(); ref++)
      printNode(*ref,indent+1);
  if (node->firstChild())
    printNode(node->firstChild(),indent+1);
  if (node->secondChild())
    printNode(node->secondChild(),indent+1);
  if (node->thirdChild())
    printNode(node->thirdChild(),indent+1);
}


void BB2DagMapper::optimize() {
  //generate code for all greatest elements
  for (DagNodes::iterator n=greatest_->begin(); n != greatest_->end(); n++)
    optimizeNode(*n);
}

void BB2DagMapper::optimizeNode(DagNode* node) {
  //if code for the node is already generated, return
  if (node->generated())
    return;
  //generate code for the references first
  DagNodes* refs = node->references();
  for (DagNodes::iterator ref=refs->begin(); ref!=refs->end(); ref++) {
    optimizeNode(*ref);
  }
  //generate code for the children
  if (node->firstChild())
    optimizeNode(node->firstChild());
  if (node->secondChild())
    optimizeNode(node->secondChild());
  if (node->thirdChild())
    optimizeNode(node->thirdChild());
  node->setGenerated();
  optimizer_->accept(node);
}
