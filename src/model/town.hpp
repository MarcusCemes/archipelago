
// archipelago - model/town.hpp
// Town-related logic, such as ENJ, CI and MTA calculations

#ifndef MODEL_TOWN_H
#define MODEL_TOWN_H

#include <map>
#include <set>
#include <vector>

#include "node.hpp"

using node::Link;
using node::Node;

namespace town {

/* === CLASSES === */

/**
 * A high level class object to manage a Town. Stores nodes and links in an optimised
 * data structure.
 *
 * Insertion/modification operations that make sense on a town-level may throw
 * errors if a physically incorrect situation arrives, such as duplicate node or a
 * superposition between town members.
 */
class Town {
 public:
  Town(std::vector<Node> nodes = std::vector<Node>(),
       std::vector<Link> links = std::vector<Link>());

  /* Accessors/Manipulators */

  /**
   * Add a Node to the town.
   * @throw If the node exists or causes a superposition with existing town members
   */
  void addNode(const Node& node, const double safetyDistance = 0.);

  /** Returns a constant pointer to the node instance, or nullptr */
  const Node* getNode(const unsigned uid) const;

  /** Returns a list of node uids that are a part of the town */
  std::vector<unsigned> getNodes() const;

  /** Removes a node by uid from the town. Does not check if the node exists */
  void removeNode(const unsigned uid);

  /**
   * Adds a link to the town
   * @throw If the link's nodes are not a part of the town, or a superposition occurs
   */
  void addLink(const Link& link);

  /** Whether the town contains a certain link between nodes (uid order-independent) */
  bool hasLink(const Link& link) const;

  /**
   * Get a list of nodes that are linked to the given node
   * @throw If the node is not a part of the town
   */
  std::vector<unsigned> getLinkedNodes(const unsigned uid) const;

  /** Removes a link from the town. Does not check if the link exists */
  void removeLink(const Link& link);

 private:
  /* Attributes */

  /** A uid-sorted map of nodes belonging to the town */
  std::map<unsigned, Node> nodes;

  /** A list of Link instances that are part of the town */
  std::vector<Link> links;

  /* Methods */

  /** Checks whether the given node intersects any town links */
  void checkNodeSuperposition(const Node& node, const double safetyDistance = 0.);

  /** Checks whether the given node intersects any town links */
  void checkLinkSuperposition(const Node& node, const double safetyDistance = 0.);
  /** Checks whether the given link intersects any town nodes */
  void checkLinkSuperposition(const Link& link, const double safetyDistance = 0.);
};

/* === FUNCTIONS === */

/** Start with an empty town */
void start();

/** Read the given file and parse the town */
void start(char* path);

}  // namespace town

#endif
