#include <sstream>

#include "number.h"

ostream& Number::prettyPrint(ostream& os,int indent) const {
	while(indent--)
	   os << " ";
	os << value_;
	return os;
}

string Number::toString() const {
   stringstream s;
   s << value_;
   return s.str();
}
