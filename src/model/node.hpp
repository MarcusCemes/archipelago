// archipelago - model/node.hpp
// Classes and tools for node management

#ifndef MODEL_NODE_H
#define MODEL_NODE_H

#include "tools.hpp"

namespace node {

/* === CLASSES === */

enum NodeType { HOUSING, TRANSPORT, PRODUCTION };

/**
 * Class that represents a node with a unique identifier,
 * a position and a capacity.
 */
class Node {
 public:
  Node(NodeType type, unsigned uid, tools::Vec2 position, unsigned capacity);

  /* Accessors/Manipulators */

  unsigned getUid() const;
  void setUid(unsigned uid);

  NodeType getType() const;
  void setType(NodeType type);

  tools::Vec2 getPosition() const;
  void setPosition(tools::Vec2 position);

  unsigned getCapacity() const;
  void setCapacity(unsigned capacity);

  /* Methods */

  unsigned radius() const;

 private:
  NodeType type;
  unsigned uid;
  tools::Vec2 position;
  unsigned capacity;
};

/**
 * Class that represents a link between nodes.
 * A link is immutable, and uid0 is always smaller than uid1.
 * WARNING: UID SORTED NOT IMPLEMENTED YET
 */
class Link {
 public:
  Link() = delete;
  Link(unsigned uid0, unsigned uid1);

  /* Accessors/Manipulators */

  unsigned getUid0() const;
  unsigned getUid1() const;

  /** Compare two link objects. Also checks with values swapped */
  bool operator==(const Link& link) const;

 private:
  unsigned uid0;
  unsigned uid1;
};

}  // namespace node

#endif
