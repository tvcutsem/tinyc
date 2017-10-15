#include "dagnode.h"
#include "../instructions/assigni.h"

DagNode::DagNode(Instruction* original,
                 DagNode* first,
		 DagNode* second,
		 DagNode* third):
  targets_    (new RegisterList),
  instr_      (original),
  firstChild_ (first),
  secondChild_(second),
  thirdChild_ (third),
  killed_     (false),
  generated_  (false),
  references_ (new DagNodes) { 
}

DagNode::~DagNode() {
  delete targets_;
  delete references_;
}

void DagNode::setChosenTarget(Register* chosen) {
  chosenTarget_ = chosen;
}

void DagNode::kill() {
  killed_ = true;
}

void DagNode::setGenerated() {
  generated_ = true;
}


void DagNode::addReference(DagNode* node) {
  references_->push_back(node);
}
