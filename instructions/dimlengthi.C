#include "dimlengthi.h"

#include <sstream>

using namespace std;

DimLengthI::~DimLengthI() {
  delete array_;
  delete dim_;
}

string DimLengthI::toString() const {
  stringstream ss;
  ss << *(getTarget()->name()) << " = DIMLENGTH ";
  ss << *(array_->name()) << "," << (dim_->value());
  return ss.str();
}
