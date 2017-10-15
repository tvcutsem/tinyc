#ifndef BBMAPPING_H
#define BBMAPPING_H

#include "label.h"
#include "instruction.h"
#include "instructions.h"

#include <list>
#include <vector>
#include <map>
#include <string>

using namespace std;

typedef vector<Instruction*>   BasicBlock;
typedef BasicBlock::iterator   bbiterator;
typedef list<BasicBlock*>      BasicBlocks;
typedef BasicBlocks::iterator  BasicBlocksit;
typedef std::map<Label, Label> LabelMapping;

/**
 * Optimizes the given instructionstream for the given function.
 * When finished, the given instructionstream will be empty and can safely
 * be deleted. The original instructions will be optimized and transferred
 * to the optimized parameter.
 */
void optimize(Instructions& instructions, Instructions& optimized,
              string& funname, bool printDag);

/**
 * The basic block mapping maps a list of instructions to a list of
 * basic blocks (ie a partitioning of instructions.
 *
 * When given a list of instructions, first, it is determined which
 * instructions are so called 'leaders':
 *   I[0] is a leader
 *   I[s] is a leader iff there is a jump instruction I[k] with I[k].destination = s
 *   I[s] is a leader iff I[s-1] is a jump instruction
 *
 * After mapping the instruction list to a set of basic blocks,
 * the instruction list may be deleted.
 */

void toBasicBlocks(Instructions& instructions,BasicBlocks& output);


/**
 * Mark all leaders in a given instructionstream. Necessary to cut
 * the stream down into smaller so called 'basic blocks'.
 */
void identifyLeaders(Instructions* instructions);


/**
 * Remaps the set of basic blocks to a list of instructions,
 * presumably after optimization has taken place.
 */
void toInstructions(BasicBlocks& bbs, Instructions& output);


/**
 * Relocate a given instruction stream by replacing all jump addresses
 * of GOTO instructions by new addresses, as specified by the given mapping,
 * which maps old to new addresses.
 */
void relocateCode(Instructions& instr, LabelMapping& addresses);

#endif
