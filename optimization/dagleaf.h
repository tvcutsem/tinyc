#ifndef DAG_LEAF_H
#define DAG_LEAF_H

#include "dagnode.h"
#include "../instructions/register.h"

/**
 * A DagLeaf is a leaf in the intermediate code DAG 
 */
class DagLeaf: public DagNode {

  public:

    DagLeaf(Register* identifier):
      DagNode(0,0,0,0), identifier_(identifier) { }

    Register* identifier() const { return identifier_; }

  private:

    Register* identifier_;

};

#endif
