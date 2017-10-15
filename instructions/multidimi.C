#include "multidimi.h"

#include <sstream>

MultiDimI::~MultiDimI() {
  delete array_;
  delete dimension_;
}

string MultiDimI::toString() const {
  stringstream ss;
  ss << "MULTIDIM " << *(array_->name()) << " " << *(dimension_->name());
  return ss.str();
}
