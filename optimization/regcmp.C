#include "regcmp.h"

bool RegCompare::operator()(Register* r1, Register* r2) const {
  return ((*r1->name()) < (*r2->name()));
}
