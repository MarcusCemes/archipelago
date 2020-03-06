// archipelago - model/node.hpp
// Classes and tools for node management

#ifndef MODEL_NODE_H
#define MODEL_NODE_H

#include "tools.hpp"

namespace node {

/* === CLASSES === */

enum NodeType { HOUSING, TRANSPORT, PRODUCTION };

/**
 * Abstract class that represents a node with a unique identifier,
 * a position and a capacity.
 */
class Node {
 public:
  Node(NodeType type, int uid, tools::Position position, int capacity);

  /* Accessors/Manipulators */

  int getUid();
  void setUid(int uid);

  NodeType getType();
  void setType(NodeType type);

  tools::Position getPos();
  void setPos(tools::Position position);

  int getCapacity();
  void setCapacity(int capacity);

  /* Methods */

  int getRadius();

 private:
  NodeType type;
  int uid;
  tools::Position position;
  int capacity;
};

}

#endif
