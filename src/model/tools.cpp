// archipelago - model/tools.cpp
// Tools for point, vector, segment and circle manipulation.

#include "tools.hpp"

#include <algorithm>  // min()
#include <cmath>      // pow(), sqrt()
#include <sstream>    // double formatting
#include <string>     // toString()

namespace tools {

constexpr int INVERSE_FACTOR(-2);

/* === VECTOR === */

/* == Accessors/Modifiers/Methods */

Vec2::Vec2() : x(0.), y(0.) {}
Vec2::Vec2(double x, double y) : x(x), y(y) {}

double Vec2::getX() const { return x; }
void Vec2::setX(double newX) { x = newX; }

double Vec2::getY() const { return y; }
void Vec2::setY(double newY) { y = newY; }

double Vec2::norm() const {
  return std::sqrt((*this) * (*this));  // dot-product
}

Vec2 Vec2::inverse() const {
  return (*this) * std::pow((*this).norm(), INVERSE_FACTOR);
}

Vec2 Vec2::project(const Vec2& ontoVector) const {
  double projectionFactor(((*this) * ontoVector) / (ontoVector * ontoVector));
  return ontoVector * projectionFactor;
}

std::string Vec2::toString() const {
  std::stringstream formatted;
  formatted << "(" << x << ", " << y << ")";
  return formatted.str();
}

/* == Operators == */

const Vec2& Vec2::operator+=(const Vec2& vector) {
  x += vector.x;
  y += vector.y;
  return *this;
}
const Vec2& Vec2::operator-=(const Vec2& vector) {
  x -= vector.x;
  y -= vector.y;
  return *this;
}
/** Multiplies the vector by a scalar value */
const Vec2& Vec2::operator*=(const double& factor) {
  x *= factor;
  y *= factor;
  return *this;
}

Vec2 operator+(Vec2 vector1, const Vec2& vector2) { return vector1 += vector2; }
Vec2 operator-(Vec2 vector1, const Vec2& vector2) { return vector1 -= vector2; }
/** Multiplies the vector by a scalar value */
Vec2 operator*(Vec2 vector, const double& factor) { return vector *= factor; }
/** Calculates the dot product between two vectors */
double operator*(Vec2 vector1, const Vec2& vector2) {
  return (vector1.getX() * vector2.getX()) + (vector1.getY() * vector2.getY());
}
std::ostream& operator<<(std::ostream& stream, const Vec2& vector) {
  stream << vector.toString();
  return stream;
}

/** === RENDER HELPERS === */

Circle::Circle(const Vec2& position, const unsigned radius)
    : position(position), radius(radius) {}

const Vec2& Circle::getPosition() const { return position; }
unsigned Circle::getRadius() const { return radius; }

Line::Line(const Vec2& pointA, const Vec2& pointB) : pointA(pointA), pointB(pointB) {}

const Vec2& Line::getPointA() const { return pointA; }
const Vec2& Line::getPointB() const { return pointB; }

Polygon4::Polygon4(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d)
    : a(a), b(b), c(c), d(d) {}
const Vec2& Polygon4::getA() const { return a; }
const Vec2& Polygon4::getB() const { return b; }
const Vec2& Polygon4::getC() const { return c; }
const Vec2& Polygon4::getD() const { return d; }

/* === FUNCTIONS === */

double minPointLineDistance(const Vec2& point, const Vec2& lineA, const Vec2& lineB) {
  Vec2 vecAP(point - lineA);
  Vec2 vecAB(lineB - lineA);

  // Projection of the point onto the line
  Vec2 vecAX(vecAP.project(vecAB));
  return (vecAP - vecAX).norm();
}

double minPointSegmentDistance(const Vec2& point, const Vec2& segmentA,
                               const Vec2& segmentB) {
  Vec2 vecAB(segmentB - segmentA);
  Vec2 vecAP(point - segmentA);
  Vec2 vecBP(point - segmentB);

  // Project both segment-point vectors onto the segment
  Vec2 vecAX(vecAP.project(vecAB));
  Vec2 vecBX(vecBP.project(vecAB));

  double segmentNorm(vecAB.norm());
  if ((vecAX.norm() < segmentNorm) && (vecBX.norm() < segmentNorm)) {
    // the closest distance is  somewhere on the segment
    return (vecAP - vecAX).norm();
  }

  // the closeest point is one of the segment defining points
  return std::min(vecAP.norm(), vecBP.norm());
}

}  // namespace tools
