// archipelago - model/node.cpp
// Classes and tools for node management

#include "node.hpp"

#include <cmath>

#include "constants.hpp"
#include "error.hpp"

/* === CLASSES === */

namespace node {

/* == Node == */

Node::Node(NodeType type, unsigned newUid, tools::Vec2 position,
           unsigned initialCapacity)
    : type(type), position(position) {
  setUid(newUid);
  setCapacity(initialCapacity);
}

/** STUB */
void Node::render(tools::RenderContext& ctx) const {
  ctx.draw(tools::Circle(position, radius()));
}

unsigned Node::getUid() const { return uid; }
void Node::setUid(unsigned newUid) {
  if (newUid == NO_LINK) throw error::reserved_uid();
  uid = newUid;
}

NodeType Node::getType() const { return type; }
void Node::setType(NodeType newType) { type = newType; }

tools::Vec2 Node::getPosition() const { return position; }
void Node::setPosition(tools::Vec2 newPosition) { position = newPosition; }

unsigned Node::getCapacity() const { return capacity; }
void Node::setCapacity(unsigned newCapacity) {
  if (newCapacity < MIN_CAPACITY) throw error::too_little_capacity(newCapacity);
  if (newCapacity > MAX_CAPACITY) throw error::too_much_capacity(newCapacity);
  capacity = newCapacity;
}

double Node::radius() const { return sqrt(capacity); }

bool Node::getIn() const { return in; }
void Node::setIn(bool newIn) { in = newIn; }

double Node::getAccess() const { return access; }
void Node::setAccess(double newAccess) { access = newAccess; }

double Node::getParent() const { return parent; }
void Node::setParent(unsigned newParent) { parent = newParent; }

/* == Link == */

Link::Link(unsigned uid0, unsigned uid1) { setUids(uid0, uid1); }

// void Link::render(tools::RenderContext& ctx) const {
//   // STUB TODO remove
// }

unsigned Link::getUid0() const { return uid0; }
unsigned Link::getUid1() const { return uid1; }

void Link::setUids(unsigned newUid0, unsigned newUid1) {
  if (newUid0 == newUid1) throw error::self_link_node(newUid0);
  if (newUid0 < newUid1) {
    uid0 = newUid0;
    uid1 = newUid1;
  } else {
    uid0 = newUid1;
    uid1 = newUid0;
  }
}

bool Link::operator==(const Link& link) const {
  return (uid0 == link.getUid0() && uid1 == link.getUid1()) ||
         (uid1 == link.getUid0() && uid0 == link.getUid1());
}

}  // namespace node
