#include "multiarrayi.h"

MultiArrayI::~MultiArrayI() {
  delete array_;
}

string MultiArrayI::toString() const {
  return string("MULTIARRAY ") + *(array_->name());
}
