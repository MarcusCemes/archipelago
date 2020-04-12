// archipelago - model/node.hpp
// Classes and tools for node management

#ifndef MODEL_NODE_H
#define MODEL_NODE_H

#include "tools.hpp"

namespace node {

/* === CLASSES === */

enum NodeType { HOUSING, TRANSPORT, PRODUCTION };

/**
 * A high level class object that represents a district (town node).
 *
 * Stores an immutable uid, a position and capacity, with methods to calculate
 * properties such as the radius.
 *
 * When creating/updating attributes, the method may throw an error if an
 * inconsistency is encountered, such as an incorrect capacity or invalid uid.
 * A Node has no conception of a town, and will not check for duplicate ids.
 */
class Node : public tools::Renderable {
 public:
  Node() = delete;
  Node(NodeType type, unsigned uid, tools::Vec2 position, unsigned capacity);

  void render(tools::RenderContext& context) const override;

  /* Accessors/Manipulators */

  unsigned getUid() const;

  NodeType getType() const;
  void setType(NodeType type);

  tools::Vec2 getPosition() const;
  void setPosition(tools::Vec2 position);

  unsigned getCapacity() const;
  /**
   * @throws IF the capacity is not valid
   */
  void setCapacity(unsigned capacity);

  /* Methods */

  /** Calculate the Node's radius, based on its current capacity */
  double radius() const;

 private:
  NodeType type;
  unsigned uid;
  tools::Vec2 position;
  unsigned capacity;

  /** Validate and set the uid. This should be immutable during the lifetime */
  void setUid(unsigned uid);
};

/**
 * A high level class that represents a connection between districts. The order of
 * uids is not preserved, and uid0 is always smaller than uid1.
 *
 * If the uids are equivalent, the constructor will throw an error. This class is
 * overloaded with the `==` operator to allow easy link-link comparisons.
 */
class Link {
 public:
  Link() = delete;
  Link(unsigned uid0, unsigned uid1);

  // void render(tools::RenderContext& context) const override; // TODO remove

  /* Accessors/Manipulators */

  unsigned getUid0() const;
  unsigned getUid1() const;

  /** Compare two link objects. Also checks with values swapped */
  bool operator==(const Link& link) const;

 private:
  unsigned uid0;
  unsigned uid1;

  void setUids(const unsigned uid0, const unsigned uid1);
};

}  // namespace node

#endif
