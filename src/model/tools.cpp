// archipelago - model/tools.cpp
// Tools for point, vector, segment and circle manipulation.

#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

#include "tools.hpp"

namespace tools {

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

Vec2 Vec2::inverse() const { return (*this) * std::pow((*this).norm(), -2); }

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

/* === FUNCTIONS === */

double minPointLineDistance(Vec2 point, Vec2 lineA, Vec2 lineB) {
  Vec2 vecAP(point - lineA);
  Vec2 vecAB(lineB - lineA);

  // Projection of the point onto the line
  Vec2 vecAX(vecAP.project(vecAB));
  return (vecAP - vecAX).norm();
}

double minPointSegment(Vec2 point, Vec2 pointA, Vec2 pointB) {
  Vec2 vecAB(pointB - pointA);
  Vec2 vecAP(point - pointA);
  Vec2 vecBP(point - pointB);
  Vec2 vecAX(vecAP.project(vecAB));
  Vec2 vecBX(vecBP.project(vecAB));
  if ((vecAX.norm() < vecAB.norm()) and (vecBX.norm() < vecAB.norm())) { 
    return (vecAP - vecAX).norm(); 
  }
  else { return min<double>(vecAP.norm(), vecBP.norm()); }
}

}  // namespace tools

