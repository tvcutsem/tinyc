#include "node.h"

#include "../tinyparser.h"

Node::Node() { lineNo_ = parser_->lineNo(); }

void Node::setParser(TinyParser& parser) {
  parser_ = &parser;
}

TinyParser* Node::parser_ = 0;

ostream& operator<<(ostream& os, Node* node) {
  os << node->toString() << endl;
  return os;
}
