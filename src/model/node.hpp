// archipelago - model/node.hpp
// Classes and tools for node management

#include "tools.hpp"


/* === CLASSES === */

enum NodeType {
  HOUSING,
  TRANSPORT,
  PRODUCTION
};

/**
 * Abstract class that represents a node with a unique identifier,
 * a position and a capacity.
 */
class Node {
public:
  Node(int uid, Position position, int capacity);

  /* Accessors/Manipulators */

  virtual NodeType getType() = 0; // set by subclasses

  int getUid();

  Position getPos();
  void setPos(Position position);

  int getCapacity();
  void setCapacity(int capacity);

  /* Methods */

  int getRadius();

private:
  NodeType type;
  int uid;
  Position position;
  int capacity;
};

class Housing : public Node {
public:
  Housing(int uid, Position position, int capacity);
  NodeType getType();
};

class Transport : public Node {
public:
  Transport(int uid, Position position, int capacity);
  NodeType getType();
};

class Production : public Node {
public:
  Production(int uid, Position position, int capacity);
  NodeType getType();
};
