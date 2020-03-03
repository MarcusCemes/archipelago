// archipelago - model/node.cpp
// Classes and tools for node management

#include <math.h>

#include "node.hpp"


/* === CLASSES === */

Node::Node(int initialUid, Position initialPosition, int initialCapacity) {
  uid = initialUid;
  position = initialPosition;
  capacity = initialCapacity;
}

int Node::getUid() { return uid; }

Position Node::getPos() { return position; }
void Node::setPos(Position newPosition) { position = newPosition; }

int Node::getCapacity() { return capacity; }
void Node::setCapacity(int newCapacity) { capacity = newCapacity; }

int Node::getRadius() { return sqrt(capacity); }

/* SUBCLASSES */

Housing::Housing(int initialUid, Position initialPosition, int initalCapacity)
 : Node(initialUid, initialPosition, initalCapacity) {}
NodeType Housing::getType() { return HOUSING; }


 Transport::Transport(int initialUid, Position initialPosition, int initalCapacity)
 : Node(initialUid, initialPosition, initalCapacity) {}
NodeType Transport::getType() { return TRANSPORT; }


Production::Production(int initialUid, Position initialPosition, int initalCapacity)
 : Node(initialUid, initialPosition, initalCapacity) {}
NodeType Production::getType() { return PRODUCTION; }
