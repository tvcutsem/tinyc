#ifndef REG_CMP_H
#define REG_CMP_H

#include "../instructions/register.h"

/**
 * Utility abstraction used in stl structures like maps and lists
 * that contain pointers to registers. To avoid that these maps
 * use the pointers to compare these values, this struct can be used
 * to compare them according to their names
 */

struct RegCompare {

  /** compare two registers according to register names, not pointer values */
  bool operator()(Register* r1, Register* r2) const;

};

#endif
