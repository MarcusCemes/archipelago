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
  Node(NodeType type, unsigned int uid, tools::Position position,
       unsigned int capacity);

  /* Accessors/Manipulators */

  unsigned int getUid() const;
  void setUid(unsigned int uid);

  NodeType getType() const;
  void setType(NodeType type);

  tools::Position getPos() const;
  void setPos(tools::Position position);

  unsigned int getCapacity() const;
  void setCapacity(unsigned int capacity);

  /* Methods */

  unsigned int getRadius() const;

 private:
  NodeType type;
  unsigned int uid;
  tools::Position position;
  unsigned int capacity;
};

/* === DEFINITIONS === */

struct Link {
  unsigned int uid0;
  unsigned int uid1;
};

}  // namespace node

#endif
