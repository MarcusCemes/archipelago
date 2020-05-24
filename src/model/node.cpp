// archipelago v3.0.0 - architecture b2
// node.cpp - node classes and functions
// Authors: Marcus Cemes, Alexandre Dodens

#include "node.hpp"

#include <cmath>
#include <sstream>

#include "constants.hpp"
#include "error.hpp"

namespace {

/* == Constants and definitions == */

constexpr double PRODUCTION_SIGN_WIDTH(0.75);
constexpr double PRODUCTION_SIGN_HEIGHT(0.16);

constexpr double SQRT_TWO(1.41421356237);
constexpr double TWO(2.);

void drawTransport(tools::RenderContext& ctx, const tools::Vec2& position,
                   double radius);
void drawProduction(tools::RenderContext& ctx, const tools::Vec2& position,
                    double radius);

}  // namespace

namespace node {

/* === CLASSES === */

/* == Node == */

Node::Node(NodeType type, unsigned newUid, tools::Vec2 position,
           unsigned initialCapacity)
    : type(type), position(position), selected(false), highlighted(false) {
  setUid(newUid);
  setCapacity(initialCapacity);
}

void Node::render(tools::RenderContext& ctx) {
  unsigned nodeRadius(radius());

  ctx.setColour(selected ? tools::ORANGE : highlighted ? tools::GREEN : tools::BLACK);
  ctx.draw(tools::Circle(position, nodeRadius));

  switch (type) {
    case PRODUCTION:
      drawProduction(ctx, position, nodeRadius);
      break;
    case TRANSPORT:
      drawTransport(ctx, position, nodeRadius);
      break;
    default:
      break;
  }
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

bool Node::getSelected() const { return selected; }

void Node::setSelected(bool isSelected) { selected = isSelected; }

bool Node::getHighlighted() const { return highlighted; }

void Node::setHighlighted(bool isHighlighted) { highlighted = isHighlighted; }

double Node::radius() const { return sqrt(capacity); }

void Node::setRadius(unsigned newRadius) {
  capacity = newRadius * newRadius;  // inverse of sqrt
  if (capacity < MIN_CAPACITY) {
    capacity = MIN_CAPACITY;
  } else if (capacity > MAX_CAPACITY) {
    capacity = MAX_CAPACITY;
  }
}

std::string Node::toString() const {
  std::ostringstream stream;
  stream << uid << " " << position.getX() << " " << position.getY() << " "
         << getCapacity();
  return stream.str();
}

/* == Link == */

Link::Link(unsigned uid0, unsigned uid1) { setUids(uid0, uid1); }

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

namespace {

void drawProduction(tools::RenderContext& ctx, const tools::Vec2& position,
                    double radius) {
  tools::Vec2 a(position.getX() - radius * PRODUCTION_SIGN_WIDTH,
                position.getY() - radius * PRODUCTION_SIGN_HEIGHT);
  tools::Vec2 b(position.getX() + radius * PRODUCTION_SIGN_WIDTH,
                position.getY() - radius * PRODUCTION_SIGN_HEIGHT);
  tools::Vec2 c(position.getX() + radius * PRODUCTION_SIGN_WIDTH,
                position.getY() + radius * PRODUCTION_SIGN_HEIGHT);
  tools::Vec2 d(position.getX() - radius * PRODUCTION_SIGN_WIDTH,
                position.getY() + radius * PRODUCTION_SIGN_HEIGHT);
  ctx.draw(tools::Polygon4(a, b, c, d));
}

void drawTransport(tools::RenderContext& ctx, const tools::Vec2& position,
                   double radius) {
  tools::Vec2 point1(position.getX() + radius, position.getY());
  tools::Vec2 point2(position.getX() + radius * SQRT_TWO / TWO,
                     position.getY() + radius * SQRT_TWO / TWO);
  tools::Vec2 point3(position.getX(), position.getY() + radius);
  tools::Vec2 point4(position.getX() - radius * SQRT_TWO / TWO,
                     position.getY() + radius * SQRT_TWO / TWO);
  tools::Vec2 point5(position.getX() - radius, position.getY());
  tools::Vec2 point6(position.getX() - radius * SQRT_TWO / TWO,
                     position.getY() - radius * SQRT_TWO / TWO);
  tools::Vec2 point7(position.getX(), position.getY() - radius);
  tools::Vec2 point8(position.getX() + radius * SQRT_TWO / TWO,
                     position.getY() - radius * SQRT_TWO / TWO);
  ctx.draw(tools::Line(point1, point5));
  ctx.draw(tools::Line(point3, point7));
  ctx.draw(tools::Line(point2, point6));
  ctx.draw(tools::Line(point4, point8));
}

}  // namespace
