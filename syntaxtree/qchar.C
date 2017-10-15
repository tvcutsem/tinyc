#include <sstream>

#include "qchar.h"

ostream& QChar::prettyPrint(ostream& os,int indent) const {
	while(indent--)
	   os << " ";
	os << &value_;
	return os;
}

string QChar::toString() const {
   return string(&value_);
}
