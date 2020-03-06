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

  int getUid() const;
  void setUid(int uid);

  NodeType getType() const;
  void setType(NodeType type);

  tools::Position getPos() const;
  void setPos(tools::Position position);

  int getCapacity() const;
  void setCapacity(int capacity);

  /* Methods */

  int getRadius() const;

 private:
  NodeType type;
  int uid;
  tools::Position position;
  int capacity;
};

/* === DEFINITIONS === */

struct Link {
  int uid1;
  int uid2;
};

}

#endif
