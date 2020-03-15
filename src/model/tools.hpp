// archipelago - model/tools.hpp
// Tools for point, vector, segment and circle manipulation.

#ifndef MODEL_TOOLS_H
#define MODEL_TOOLS_H

#include <iostream>  // operator<< overloading

namespace tools {

/* === VECTOR === */

/** A two-dimensional vector */
class Vec2 {
 public:
  Vec2();
  Vec2(double x, double y);

  /* Accessors/Modifiers */
  double getX() const;
  void setX(double x);

  double getY() const;
  void setY(double y);

  const Vec2& operator+=(const Vec2& vector);
  const Vec2& operator-=(const Vec2& vector);
  /** Multiplies the vector by a scalar value */
  const Vec2& operator*=(const double& factor);

  /* Methods */
  /** Returns the norm of the vector */
  double norm() const;
  /** Returns a new vector which is the inverse of the current vector */
  Vec2 inverse() const;
  /** Returns a new vector that is the projection onto `the vector parameter */
  Vec2 project(const Vec2& ontoVector) const;
  std::string toString() const;

 private:
  double x;
  double y;
};

Vec2 operator+(const Vec2 vector1, const Vec2& vector2);
Vec2 operator-(const Vec2 vector1, const Vec2& vector2);
/** Multiplies the vector by a scalar value */
Vec2 operator*(const Vec2 vector, const double& factor);
/** Calculates the dot product between two vectors */
double operator*(const Vec2 vector1, const Vec2& vector2);
std::ostream& operator<<(std::ostream& stream, const Vec2& vector);

/* === FUNCTIONS === */

/**
 * Calculates the minimum distance between
 * a point and a line defined by two points
 */
double minPointLineDistance(Vec2 point, Vec2 lineA, Vec2 lineB);
double minPointSegment(Vec2 point, Vec2 pointA, Vec2 pointB);

}  // namespace tools

#endif
