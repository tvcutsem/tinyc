#ifndef LABEL_SET_H
#define LABEL_SET_H

#include "../instructions/label.h"
#include "../instructions/instruction.h"
#include "../instructions/instructions.h"

#include <set>

/**
 * A LabelSet gathers labels which have to be 'backpatched', ie
 * the location they jump to is not yet known. When the location
 * is known, a LabelSet can be traversed to set the location right.
 */

typedef set<Label> LabelSet;

typedef LabelSet::iterator Labelit;

typedef LabelSet::const_iterator ConstLabelit;

/**
 * Set the goto label of all instructions in the given instruction stream
 * whose label is present in the LabelSet to the given exit label
 */

void backpatch(const LabelSet& labels,
    	       const Instructions& instructions,
	       const Label exit);

#endif
