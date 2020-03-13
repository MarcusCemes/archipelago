// archipelago - model/tools.hpp
// Tools for point, vector, segment and circle manipulation.

#ifndef MODEL_TOOLS_H
#define MODEL_TOOLS_H

namespace tools {

/** Position in 2D space */
struct Position {
  double x;
  double y;
};

struct Vecteur {
  double x;
  double y;
};

double dist_min_point_droite(Position point, Position centre1,
                             Position centre2);

}  // namespace tools

#endif
