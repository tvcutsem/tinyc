#ifndef NODE_H
#define NODE_H

#include "../type.h"
#include "../visitors/treevisitor.h"

#include <iostream>
#include <string>

//forward declaration
class TinyParser;

using namespace std;

/**
 * Common superclass of all node classes which can constitute an
 * abstract syntax tree
 * Defines the interface to which every node must conform
 */
class Node {

   public:

      Node();

      virtual ~Node() { }

      /**
       * prettyprints a node to an ostream
       * @param indent the indentation for the node, ie the number of
       * preceding whitespace
       */
      virtual ostream& prettyPrint(ostream& os = cout,int indent=0) const = 0;

      /**
       * @return a string representing the node, used for printing
       * parse trees
       */
      virtual string toString() const = 0;

      /**
       * accepts a TreeVisitor, according to the Visitor design pattern
       */
      virtual void accept(TreeVisitor* v) = 0;

      /**
       * returns the linenumber on which this node was created
       */
      int lineno() const { return lineNo_; }

      static void setParser(TinyParser& parser);

   private:

      int lineNo_;

      /**
       * The parser constructing the parse trees containing these nodes
       */
      static TinyParser* parser_;

};

ostream& operator<<(ostream& os, Node* node);

#endif
