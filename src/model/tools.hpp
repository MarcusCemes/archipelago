// archipelago - model/tools.hpp
// Tools for point, vector, segment and circle manipulation.

#ifndef MODEL_TOOLS_H
#define MODEL_TOOLS_H

#include <iostream>  // operator<< overloading

namespace tools {

/** Constants that represent rendering colours */
enum Colour { BLACK, GREEN };

/* === VECTOR === */

/**
 * A primitive two-dimensional vector object, with overloaded operators to allow easy
 * vector-vector and vector-double manipulations.
 */
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

/* === RENDER HELPERS === */

/** An immutable circle primitive */
class Circle {
 public:
  Circle() = delete;
  Circle(const Vec2& position, const unsigned radius);
  const Vec2& getPosition() const;
  unsigned getRadius() const;

 private:
  Vec2 position;
  unsigned radius;
};

/** An immutable line primitive */
class Line {
 public:
  Line() = delete;
  Line(const Vec2& pointA, const Vec2& pointB);
  const Vec2& getPointA() const;
  const Vec2& getPointB() const;

 private:
  Vec2 pointA;
  Vec2 pointB;
};

/** An immutable four-sided polygon primitive */
class Polygon4 {
 public:
  Polygon4() = delete;
  Polygon4(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d);
  const Vec2& getA() const;
  const Vec2& getB() const;
  const Vec2& getC() const;
  const Vec2& getD() const;

 private:
  Vec2 a;
  Vec2 b;
  Vec2 c;
  Vec2 d;
};

/** An abstract class that can be implemented by a renderer */
class RenderContext {
 public:
  virtual void draw(const Circle& circle) = 0;
  virtual void draw(const Line& line) = 0;
  virtual void draw(const Polygon4& line) = 0;
  virtual void setColour(const Colour& colour) = 0;
};

/** An abstract class of an object that can be rendered */
class Renderable {
  virtual void render(tools::RenderContext& context) const = 0;
};

/* === FUNCTIONS === */

/**
 * Calculates the minimum distance between a point and a line defined by two points
 */
double minPointLineDistance(const Vec2& point, const Vec2& lineA, const Vec2& lineB);

/**
 * Calculates the minimum distance between a point and a segment defined by two points
 */
double minPointSegmentDistance(const Vec2& point, const Vec2& segmentA,
                               const Vec2& segmentB);

}  // namespace tools

#endif
