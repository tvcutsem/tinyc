#include "bbmapping.h"
#include "gotoi.h"
#include "../optimization/bb2dag.h"

#include <algorithm>
#include <iterator>
#include <stdexcept>


void optimize(Instructions& instructions, Instructions& optimized,
              string& funname, bool printDag) {
  BasicBlocks bbs;
  //first, identify the leaders of the instruction stream
  identifyLeaders(&instructions);
  //next, map them to basic blocks
  toBasicBlocks(instructions,bbs);

  //labels denoting old and new beginning of the basic blocks
  Label unoptimizedBegin;
  Label optimizedBegin(optimized.nextInstructionLabel());
  LabelMapping relocation;

  //now, optimize each of the basic blocks
  for (BasicBlocksit bb = bbs.begin(); bb != bbs.end(); bb++) {
    if ((*bb)->empty())
      continue;
    unoptimizedBegin = (**bb)[0]->getLabel();
    relocation[unoptimizedBegin] = optimizedBegin;
    BB2DagMapper m(**bb,optimized,funname);
    m.makeDAG();
    if (printDag)
      m.printDAG();
    m.optimize();
    //first, mark the beginning of the optimized basic block as a leader
    optimized[optimizedBegin]->setLeader();
    optimizedBegin = optimized.nextInstructionLabel();
  }

  //don't forget the 'last' instruction, which can also have changed address
  relocation[instructions.nextInstructionLabel()] = optimizedBegin;
  
  //finally reallocate the code to fit the new optimized labels
  relocateCode(optimized, relocation);
}


void toBasicBlocks(Instructions& instructions,BasicBlocks& output) {
  //move all instruction parts to their separate basic block
  BasicBlock* block(0);
  
  for (Instructionit i = instructions.begin(); i != instructions.end(); i++) {
    if ((*i)->isLeader()) {
      //add the old block to the output
      if (block)
	output.push_back(block);
      //create a new basic block
      block = new BasicBlock;
    }
    //move instruction from the stream to the block
    block->push_back(*i);
  }
  //don't forget to add the last block
  if (block)
    output.push_back(block);

  //remove all instructions from their instructionstream
  instructions.clear();
}


void identifyLeaders(Instructions* instructions) {
  // *i is an Instruction*
  Instructionit i(instructions->begin());
  Instructionit last(instructions->end());
  bool lastWasJump(false); //indicates whether previous instruction was a jump
  if (i != last) {
    //I[0] is a leader
    (*i)->setLeader();
  }
  while (i != last) {
    //I[s-1] was a jump?
    if (lastWasJump) {
      (*i)->setLeader();
      lastWasJump = false;
    }
    //I[s] is a jump?
    GotoI* jumpI = dynamic_cast<GotoI*>(*i);
    if (jumpI) {
      lastWasJump = true;
      Label destination = jumpI->getDestination();
      if (destination < int(instructions->size())) {
	Instruction* target = (*instructions)[destination];
	//I[s] is a leader because there exists an I[k] jump to here
	target->setLeader();
      }
    }
    i++;
  }
}


void toInstructions(BasicBlocks& bbs, Instructions& output) {
  //copy all instructions from the basic block back to the output stream
  typedef insert_iterator<Instructions> iinserter;
  for (BasicBlocksit bb = bbs.begin(); bb != bbs.end(); bb++) {
    // *bb is a BasicBlock*
    std::copy((*bb)->begin(),(*bb)->end(),iinserter(output,output.end()));
    delete (*bb);
  }
}


void relocateCode(Instructions& instr, LabelMapping& addresses) {
  //for every GOTO instruction, relocate the label by replacing the old
  //jump address by the new one, as specified by the address mapping
  for (Instructionit i = instr.begin(); i != instr.end(); i++) {
    GotoI* gotoi = dynamic_cast<GotoI*>(*i);
    //relocate only goto statements
    if (gotoi) {
      Label oldAddress(gotoi->getDestination());
      //addressMapping is a mapping from oldAddress to newAddress
      LabelMapping::iterator addressMapping(addresses.find(oldAddress));
      if (addressMapping==addresses.end())
	//new address not found, should not happen!
	throw std::logic_error(
	    "Relocation of optimized code failed: could not relocate an address");
      gotoi->setDestination(addressMapping->second);
    }
  }
}
