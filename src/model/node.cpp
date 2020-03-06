// archipelago - model/node.cpp
// Classes and tools for node management

#include <math.h>

#include "node.hpp"

using namespace node;

/* === CLASSES === */

Node::Node(NodeType nodeType, int initialUid, tools::Position initialPosition,
           int initialCapacity) {
  type = nodeType;
  uid = initialUid;
  position = initialPosition;
  capacity = initialCapacity;
}

int Node::getUid() { return uid; }
void Node::setUid(int newUid) { uid = newUid; }

NodeType Node::getType() { return type; }
void Node::setType(NodeType newType) { type = newType; }

tools::Position Node::getPos() { return position; }
void Node::setPos(tools::Position newPosition) { position = newPosition; }

int Node::getCapacity() { return capacity; }
void Node::setCapacity(int newCapacity) { capacity = newCapacity; }

int Node::getRadius() { return sqrt(capacity); }
