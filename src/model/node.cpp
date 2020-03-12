// archipelago - model/node.cpp
// Classes and tools for node management

#include <math.h>

#include "node.hpp"

using namespace node;

/* === CLASSES === */

Node::Node(NodeType nodeType, unsigned int initialUid,
           tools::Position initialPosition, unsigned int initialCapacity) {
  type = nodeType;
  uid = initialUid;
  position = initialPosition;
  capacity = initialCapacity;
}

unsigned int Node::getUid() const { return uid; }
void Node::setUid(unsigned int newUid) { uid = newUid; }

NodeType Node::getType() const { return type; }
void Node::setType(NodeType newType) { type = newType; }

tools::Position Node::getPos() const { return position; }
void Node::setPos(tools::Position newPosition) { position = newPosition; }

unsigned int Node::getCapacity() const { return capacity; }
void Node::setCapacity(unsigned int newCapacity) { capacity = newCapacity; }

unsigned int Node::getRadius() const { return sqrt(capacity); }
