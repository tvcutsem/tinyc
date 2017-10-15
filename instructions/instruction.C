#include "instruction.h"

/**
 * printing an instruction means printing its string representation
 */

ostream& operator<<(ostream& os,Instruction* i) {
  os << i->toString();
  return os;
}
