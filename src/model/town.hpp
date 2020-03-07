
// archipelago - model/town.hpp
// Town-related logic, such as ENJ, CI and MTA calculations

#ifndef MODEL_TOWN_H
#define MODEL_TOWN_H

#include <vector>

#include "node.hpp"

namespace town {

/* === CLASSES */

/** A class that represents a Town */
class Town {
 public:
  Town(std::vector<node::Node> nodes = std::vector<node::Node>(),
       std::vector<node::Link> links = std::vector<node::Link>());

  /* Accessors/Manipulators */
  std::vector<node::Node> getNodes();
  void setNodes(std::vector<node::Node> nodes);

  std::vector<node::Link> getLinks();
  void setLinks(std::vector<node::Link> links);

 private:
  /* Attributes */
  std::vector<node::Node> nodes;
  std::vector<node::Link> links;
};

/* === FUNCTIONS === */

/** Read the given file and parse the town */
town::Town loadFromFile(char *path);

}  // namespace town

#endif
