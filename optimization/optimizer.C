#include "optimizer.h"
#include "usecount.h"

//Instruction classes
#include "../instructions/instruction.h"
#include "../instructions/addressi.h"
#include "../instructions/arralloci.h"
#include "../instructions/arraccessi.h"
#include "../instructions/arrassigni.h"
#include "../instructions/arrparami.h"
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

#include <iostream>
#include <stdexcept>

Optimizer::Optimizer(Instructions& codestream,
                     string& funname):
  codestream_  (codestream),
  node_        (0),
  instruction_ (0),
  targets_     (0),
  childTarget1_(0),
  childTarget2_(0),
  childTarget3_(0),
  chosenTarget_(0),
  funName_     (funname) { }

void Optimizer::accept(DagNode* node) {
  node_ = node;
  instruction_ = node->instruction();
  targets_ = &(node->targets());
  childTarget1_ = node->firstChild()?node->firstChild()->chosenTarget():0;
  childTarget2_ = node->secondChild()?node->secondChild()->chosenTarget():0;
  childTarget3_ = node->thirdChild()?node->thirdChild()->chosenTarget():0;
  //if we're dealing with a leaf, handle it differently
  DagLeaf* leaf = dynamic_cast<DagLeaf*>(node);
  if (leaf)
    optimizeLeaf(leaf);
  else
    instruction_->accept(this);
  node->setChosenTarget(chosenTarget_);
  chosenTarget_ = 0;
}

void Optimizer::visitArrayAllocInstruction(ArrayAllocI* aloc) {
  //childTarget 1 points to a result for a previous 'non-assignment' node
  //childTarget 2 contains the array reference, childTarget 3 the length
  codestream_ << new ArrayAllocI(toArrayRegister(clone(childTarget2_)),
      				 toVarRegister(clone(childTarget3_)),
				 toNumRegister(clone(aloc->getDimension())));
  chosenTarget_ = childTarget2_;
}

void Optimizer::visitArrayAccInstruction(ArrayAccessI* aac) {
  //childTarget1 possibly points to an array assignment node
  //childTarget2 contains the arraynode, childTarget3 the indexnode
  RegisterList toUpdate;
  chooseTarget(*targets_,toUpdate);
  //chosen target will be in chosenTarget_ now
  //new ArrayAccessI(Target,Array,Index)
  codestream_ << (new ArrayAccessI(
	          	toVarRegister(clone(chosenTarget_)),
		  	toArrayRegister(clone(childTarget2_)),
		  	toVarRegister(clone(childTarget3_))));
  //update other 'live' targets to contain the value of the chosen target
  updateTargets(chosenTarget_,toUpdate);
}

void Optimizer::visitArrayAssInstruction(ArrayAssignmentI* aas) {
  //childTarget 1 contains the array reference
  //childTarget 2 contains the index reference
  //childTarget 3 contains the value reference
  //new ArrayAssignmentI(array,index,value)
  codestream_ << (new ArrayAssignmentI(
			toArrayRegister(clone(childTarget1_)),
			toVarRegister(clone(childTarget2_)),
			toVarRegister(clone(childTarget3_))));
  //chosen target is irrelevant for this kind of node
}

void Optimizer::visitBinaryAssInstruction(BinAssignmentI* bas) {
  //childTarget 1 and 2 are expected to hold the child results
  RegisterList toUpdate;
  chooseTarget(*targets_,toUpdate);
  //chosen target will be in chosenTarget_ now
  //new BinAssignmentI(Op,Target,firstArg,secondArg)
  codestream_ << (new BinAssignmentI(
			bas->getOperator(),
	          	toVarRegister(clone(chosenTarget_)),
		  	toVarRegister(clone(childTarget1_)),
		  	toVarRegister(clone(childTarget2_))));
  //update other 'live' targets to contain the value of the chosen target
  updateTargets(chosenTarget_,toUpdate);
}

void Optimizer::visitCallInstruction(CallI* call) {
  //childTarget 1 possibly points to the previous 'non-assignment' node
  //new CallI(fun,args), an exact copy of the call instruction
  codestream_ << (new CallI(call->getFunction(),call->getNrOfArgs()));
  //chosenTarget irrelevant in this context
}

void Optimizer::visitGotoInstruction(GotoI* gotoi) {
  //childTarget 1 possibly points to the previous 'non-assignment' node
  //new GotoI(destination), an exact copy of the goto instruction
  codestream_ << (new GotoI(gotoi->getDestination()));
  //chosenTarget irrelevant in this context (a goto is always last)
}

void Optimizer::visitIfInstruction(IfI* ifi) {
  //childTarget 1 possibly points to the previous 'non-assignment' node
  //childTarget 2 points to the first argument of the relational comparison
  //childTarget 3 points to the second argument of the relational comparison
  //new Ifi(Op,arg1,arg2,destination)
  codestream_ << (new IfI(ifi->getOp(),
			  toVarRegister(clone(childTarget2_)),
			  toVarRegister(clone(childTarget3_)),
			  ifi->getDestination()));
  //setting the chosenTarget is irrelevant in this context (an if is always last)
}

void Optimizer::visitLengthInstruction(LengthI* length) {
  //childTarget 1 contains the array reference
  //choose the best target and optimize by choosing a 'live' target
  RegisterList toUpdate;
  chooseTarget(*targets_,toUpdate);
  //chosen target will be in chosenTarget_ now
  //new LengthI(array)
  codestream_ << (new LengthI(toVarRegister(clone(chosenTarget_)),
      			      toArrayRegister(clone(childTarget1_))));
  //update other 'live' targets to contain the value of the chosen target
  updateTargets(chosenTarget_,toUpdate);
}

void Optimizer::visitParamInstruction(ParamI* par) {
  //childTarget 1 possibly points to a 'non-assignment' node
  //childTarget 2 contains a reference to the variable to pass
  //new ParamI(variable)
  codestream_ << (new ParamI(toVarRegister(clone(childTarget2_))));
  //setting the chosen target is irrelevant for PARAM instructions
}

void Optimizer::visitArrayParamInstruction(ArrayParamI* par) {
  //childTarget 1 possibly points to a 'non-assignment' node
  //childTarget 2 contains a reference to the array to pass
  //new ArrayParamI(array)
  codestream_ << (new ArrayParamI(toArrayRegister(clone(childTarget2_))));
  //setting the chosen target is irrelevant for ARRAYPARAM instructions
}

void Optimizer::visitReadInstruction(ReadI* read) {
  //childTarget 1 possibly points to a previous 'non-assignment' node 
  //childTarget 2 contains a reference to the variable to read
  RegisterList toUpdate;
  if (!targets_->empty()) {
    chooseTarget(*targets_,toUpdate);
    //push chosen back to the list because the read chooses his own target
    toUpdate.push_back(chosenTarget_);
  }

  //new ReadI(variable)
  codestream_ << (new ReadI(read->getSource()));
  chosenTarget_ = read->getSource();

  //update other 'live' targets to contain the value of the chosen target
  updateTargets(chosenTarget_,toUpdate);
}

void Optimizer::visitReturnInstruction(ReturnI* ret) {
  //childTarget 1 possibly points to a previous 'non-assignment' node 
  //childTarget 2 contains a reference to the variable to return
  //new ReturnI(variable)
  codestream_ << (new ReturnI(toVarRegister(clone(childTarget2_))));
  chosenTarget_ = childTarget2_;
}

void Optimizer::visitReturnValInstruction(RetValI* rval) {
  //childTarget 1 possibly points to a previous 'non-assignment' node 
  //choose the best target and optimize by choosing a 'live' target
  RegisterList toUpdate;
  chooseTarget(*targets_,toUpdate);
  //chosen target will be in chosenTarget_ now
  //new RetValI(Target)
  codestream_ << (new RetValI(toVarRegister(clone(chosenTarget_))));
  //update other 'live' targets to contain the value of the chosen target
  updateTargets(chosenTarget_,toUpdate);
}

void Optimizer::visitSimpleAssInstruction(SimpleAssignmentI* sim) {
  //cannot happen because Simple Assignments are disassembled when creating
  //the DAG: 'x = y' is usually transformed to a leaf for y with x being a target
  throw std::logic_error("Optimizer processed a SimpleAssignment Instruction");
}

void Optimizer::visitUnaryAssInstruction(UnAssignmentI* uas) {
  //childTarget 1 is expected to hold the child result
  RegisterList toUpdate;
  chooseTarget(*targets_,toUpdate);
  //chosen target will be in chosenTarget_ now
  //new UnAssignmentI(Op,Target,arg)
  codestream_ << (new UnAssignmentI(
			uas->getOperator(),
	          	toVarRegister(clone(chosenTarget_)),
		  	toVarRegister(clone(childTarget1_))));
  //update other 'live' targets to contain the value of the chosen target
  updateTargets(chosenTarget_,toUpdate);
}

void Optimizer::visitWriteInstruction(WriteI* write) {
  //childTarget1_ is unused: child pointing to previous 'non-assignment' node
  //childTarget2_ must contain the register to write out, child3 is nill
  codestream_ << (new WriteI(toVarRegister(clone(childTarget2_))));
  chosenTarget_ = childTarget2_;
}

void Optimizer::visitDimLengthI(DimLengthI* dim) {
  //childTarget 1 possibly points to a previous 'non-assignment' node 
  //choose the best target and optimize by choosing a 'live' target
  RegisterList toUpdate;
  chooseTarget(*targets_,toUpdate);
  //chosen target will be in chosenTarget_ now
  //new DimLengthI(Target,oldarray,olddimension)
  codestream_ << (new DimLengthI(toVarRegister(clone(chosenTarget_)),
	                         dim->getArray(), dim->getDimension()));
  //update other 'live' targets to contain the value of the chosen target
  updateTargets(chosenTarget_,toUpdate);
}


void Optimizer::visitMultiDimInstruction(MultiDimI* multi) {
  //childTarget 1 possibly points to a previous 'non-assignment' node 
  //childTarget 2 points to the dimension length target

  //new MultiDimI(oldarray,oldnth,newdimension)
  codestream_ << (new MultiDimI(toArrayRegister(clone(multi->getArray())),
	                        toNumRegister(clone(multi->getNthDimension())),
	                        toVarRegister(clone(childTarget2_))));
  chosenTarget_ = multi->getArray();
}


void Optimizer::visitMultiArrayInstruction(MultiArrayI* marr) {
  //childTarget 1 possibly points to a previous 'non-assignment' node 
  //create an exact copy of the old multiarray instruction
  codestream_ << (new MultiArrayI(toArrayRegister(clone(marr->getArray()))));
  chosenTarget_ = marr->getArray();
}


Register* Optimizer::clone(Register* r) {
  r->accept(this);
  if (r->isTemporary())
    UseCount::instance().incCount(funName_,r);
  return clone_;
}

//used for cloning
void Optimizer::visitArrayRegister(ArrayRegister* areg) {
  clone_ = new ArrayRegister(*areg);
}

void Optimizer::visitFunRegister(FunRegister* funreg) {
  clone_ = new FunRegister(*funreg);
}

void Optimizer::visitVarRegister(VarRegister* varreg) {
  if (varreg->isTemporary())
    clone_ = new TempRegister(*varreg);
  else
    clone_ = new VarRegister(*varreg);
}

void Optimizer::visitNumRegister(NumRegister* numreg) {
  clone_ = new NumRegister(*numreg);
}

void Optimizer::visitQCharRegister(QCharRegister* creg) {
  clone_ = new QCharRegister(*creg);
}

void Optimizer::visitLeader(Instruction* i) {
}

void Optimizer::optimizeLeaf(DagLeaf* leaf) {
  //if the leaf has no targets, the chosen target is just
  //the leaf's identifier and no code is generated
  if (targets_->size()==0) {
    chosenTarget_ = leaf->identifier();
  } else {
    //otherwise, generate an assignment for each target
    RegisterList toUpdate;
    chooseTarget(*targets_,toUpdate);
    //instead of using target, we use the identifier as the source
    //but first we'll have to assign to that one too of course
    codestream_ << (new SimpleAssignmentI(toVarRegister(clone(chosenTarget_)),
	  				  toVarRegister(clone(leaf->identifier()))));
    updateTargets(leaf->identifier(),toUpdate);
  }
}

VarRegister* Optimizer::toVarRegister(Register* reg) {
  VarRegister* var = dynamic_cast<VarRegister*>(reg);
  if (var)
    return var;
  else
    throw std::logic_error("Optimizer: downcast from Register to VarRegister failed!");
}

ArrayRegister* Optimizer::toArrayRegister(Register* reg) {
  ArrayRegister* arr = dynamic_cast<ArrayRegister*>(reg);
  if (arr)
    return arr;
  else
    throw std::logic_error("Optimizer: downcast from Register to ArrayRegister failed!");
}

NumRegister* Optimizer::toNumRegister(Register* reg) {
  NumRegister* num = dynamic_cast<NumRegister*>(reg);
  if (num)
    return num;
  else
    throw std::logic_error("Optimizer: downcast from Register to NumRegister failed!");
}

void Optimizer::chooseTarget(RegisterList& targets, RegisterList& updates) {
  RegisterList::iterator target(targets.begin());
  //first search for the first 'live' variable
  while ((target!=targets.end()) && (*target)->isTemporary())
    target++;
  //if target is at the end, there are no 'live' variables, thus, choose
  //the first temporary
  if (target==targets.end()) {
    chosenTarget_ = *targets.begin();
    return;
  }
  
  chosenTarget_ = *target;
  //next, copy all remaining 'live' variables to the update list
  target++;
  while (target!=targets.end()) {
    if (!(*target)->isTemporary())
      updates.push_back(*target);
    target++;
  }
}

void Optimizer::updateTargets(Register* source, RegisterList& updates) {
  for (RegisterList::iterator reg = updates.begin(); reg != updates.end(); reg++) {
    // *reg is a Register*
    codestream_ << (new SimpleAssignmentI(toVarRegister(clone(*reg)),
	  				  toVarRegister(clone(source))));
  }
}
