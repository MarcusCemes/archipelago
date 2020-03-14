// archipelago - model/node.cpp
// Classes and tools for node management

#include <math.h>

#include "node.hpp"

/* === CLASSES === */

namespace node {

/* == Node == */

Node::Node(NodeType nodeType, unsigned initialUid, tools::Vec2 initialPosition,
           unsigned initialCapacity) {
  type = nodeType;
  uid = initialUid;
  position = initialPosition;
  capacity = initialCapacity;
}

unsigned Node::getUid() const { return uid; }
void Node::setUid(unsigned newUid) { uid = newUid; }

NodeType Node::getType() const { return type; }
void Node::setType(NodeType newType) { type = newType; }

tools::Vec2 Node::getPosition() const { return position; }
void Node::setPosition(tools::Vec2 newPosition) { position = newPosition; }

unsigned Node::getCapacity() const { return capacity; }
void Node::setCapacity(unsigned newCapacity) { capacity = newCapacity; }

unsigned Node::radius() const { return sqrt(capacity); }

/* == Link == */

Link::Link(unsigned uid0, unsigned uid1) : uid0(uid0), uid1(uid1) {}

unsigned Link::getUid0() const { return uid0; }
unsigned Link::getUid1() const { return uid1; }

bool Link::operator==(const Link& link) const {
  return (uid0 == link.getUid0() && uid1 == link.getUid1()) ||
         (uid1 == link.getUid0() && uid0 == link.getUid1());
}

}  // namespace node
