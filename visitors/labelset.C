#include "labelset.h"
#include "../instructions/gotoi.h"

#include <stdexcept>

void backpatch(const LabelSet& labels,
    	       const Instructions& instructions,
	       const Label exit) {
  for (ConstInstructionit i = instructions.begin() ; i != instructions.end() ; i++) {
    // *i is an Instruction*
    ConstLabelit instr = labels.find((*i)->getLabel());
    if (instr != labels.end()) {
      //instruction's label is in the labelset, so it has to be backpatched
      GotoI* gotoinstr = dynamic_cast<GotoI*>(*i);
      if (!gotoinstr)
	throw std::logic_error("Backpatch can only backpatch goto instructions!");
      gotoinstr->setDestination(exit);
    }
  }
}
