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
  Node(int uid, tools::Position position, int capacity);

  /* Accessors/Manipulators */

  virtual NodeType getType() = 0;  // set by subclasses

  int getUid();

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

class Housing : public Node {
 public:
  Housing(int uid, tools::Position position, int capacity);
  NodeType getType();
};

class Transport : public Node {
 public:
  Transport(int uid, tools::Position position, int capacity);
  NodeType getType();
};

class Production : public Node {
 public:
  Production(int uid, tools::Position position, int capacity);
  NodeType getType();
};

}

#endif
