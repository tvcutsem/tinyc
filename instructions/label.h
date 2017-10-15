#ifndef LABEL_H
#define LABEL_H

#include <string>

typedef int Label;

/**
 * Returns the label logically following the given one
 */
Label next(Label l);

/**
 * Indicates a new basic block is processed, so the label count is reset
 */
void beginBlock();

/**
 * Returns the next label, relative to one given basic block!
 */
Label next();

/**
 * Conversion from a label to a string
 */
std::string label2str(Label l);

#endif
