#include "instructions.h"

Instructions& Instructions::operator<<(Instruction* i) {
  vector<Instruction*>::push_back(i);
  i->setLabel(nextLabel_);
  nextLabel_++;
  return *this;
}

Instructions::~Instructions() {
  for (Instructionit i = begin(); i!= end(); i++) {
    // *i is an Instruction*
    delete (*i);
  }
}

Instruction* Instructions::operator[](const Label l) const {
  return vector<Instruction*>::operator[](l);
}

Label Instructions::nextInstructionLabel() const {
  return nextLabel_;
}

ostream& operator<<(ostream& os,Instructions& instructions) {
  for (Instructionit i = instructions.begin(); i!=instructions.end(); i++) {
    	//*i is an Instruction*
  	//mark leaders with a ">" sign
  	os << ((*i)->isLeader() ? "> " : "  "); 
	os << (*i)->getLabel() << "\t"  << *i << endl;
  }
  return os;
}

