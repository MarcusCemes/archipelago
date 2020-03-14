// archipelago - model/tools.cpp
// Tools for point, vector, segment and circle manipulation.

#include <cmath>

#include "tools.hpp"

namespace tools {

double dist_min_point_droite(Position point, Position centre1,
                             Position centre2) {
  Vecteur v, w, a;
  double norme_v, norme_w, norme_a;
  double dist_min;
  v.x = centre2.x - centre1.x;
  v.y = centre2.y - centre1.y;
  norme_v = sqrt(v.x * v.x + v.y * v.y);
  w.x = ((point.x - centre1.x) * v.x) / norme_v;
  w.y = ((point.y - centre1.y) * v.y) / norme_v;
  norme_w = sqrt(w.x * w.x + w.y * w.y);
  a.x = point.x - centre1.x;
  a.y = point.y - centre1.y;
  norme_a = sqrt(a.x * a.x + a.y * a.y);
  dist_min = (norme_a * norme_a) - (norme_w * norme_w);
  return dist_min;
}

}  // namespace tools