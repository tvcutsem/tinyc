#include "label.h"

#include <sstream>

static int count = 0;

Label next(Label l) {
  return l++;
}

void beginFunction() {
  count = 0;
}

Label next() {
  return count++;
}

std::string label2str(Label l) {
  std::stringstream ss;
  ss << "L" << l;
  return ss.str();
}
