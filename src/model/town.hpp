// archipelago v2.0.0 - architecture b2
// town.hpp - town classes and functions
// Authors: Marcus Cemes, Alexandre Dodens

#ifndef MODEL_TOWN_H
#define MODEL_TOWN_H

#include <map>
#include <memory>
#include <vector>

#include "node.hpp"
#include "tools.hpp"

namespace {

constexpr double DEFAULT_SAFETY(0.);

}

namespace town {

/* === DEFINITIONS === */

typedef std::unique_ptr<std::vector<unsigned>> Path;

/** Represents a result from a path finding operation. If a path is not found, path is
 * a null pointer. */
struct PathFindingResult {
  bool success;
  Path path;
  double distance;
};

/* === CLASSES === */

/**
 * A high level class object to manage a Town. Stores nodes and links in an optimised
 * data structure.
 *
 * Insertion/modification operations that make sense on a town-level may throw
 * errors if a physically incorrect situation arrives, such as a duplicate node uid
 * or a superposition between town members.
 */
class Town : public tools::Renderable {
 public:
  Town(std::vector<node::Node> nodes = std::vector<node::Node>(),
       std::vector<node::Link> links = std::vector<node::Link>());

  void render(tools::RenderContext& context) override;

  /* Accessors/Manipulators */

  /**
   * Add a Node to the town.
   * @throws If the node exists or causes a superposition with existing town members
   */
  void addNode(const node::Node& node, const double safetyDistance = DEFAULT_SAFETY);

  /** Returns a constant pointer to the node instance, or nullptr */
  const node::Node* getNode(const unsigned uid) const;

  /** Returns a non-constant pointer to the node instance, or nullptr */
  node::Node* getModifiableNode(const unsigned uid);

  /** Returns a list of node uids that are a part of the town */
  std::vector<unsigned> getNodes() const;

  /** Removes a node by uid from the town. Does not check if the node exists */
  void removeNode(unsigned uid);

  /** Moves a node to a new position, or throws a string error on collision */
  void moveNode(unsigned uid, const tools::Vec2& newPosition);

  /** Resizes the given node. Careful! The new size is the radius, not the capacity */
  void resizeNode(unsigned uid, unsigned newRadius);

  /**
   * Adds a link to the town
   * @throws If the link's nodes are not a part of the town, or a superposition occurs
   */
  void addLink(const node::Link& link);

  /** Whether the town contains a certain link between nodes (uid order-independent) */
  bool hasLink(const node::Link& link) const;

  /** Returns an immutably referenced array of links in the town */
  const std::vector<node::Link>* getLinks() const;

  /**
   * Get a list of nodes that are linked to the given node
   * @throws If the node is not a part of the town
   */
  std::vector<unsigned> getLinkedNodes(const unsigned uid) const;

  /** Removes a link from the town. Does not check if the link exists */
  void removeLink(const node::Link& link);

  /** Calculate the town ENJ index */
  double enj();
  /** Calculate the town CI index */
  double ci();
  /** Calculate the town MTA index */
  double mta();

  /**
   * Execute a pathfinding algorithm from an origin node to the closest node of a
   * certain type. Returns a result containing whether a valid path was found and an
   * accompanying list of node UIDs in the path if yes.
   *
   * The current implementation of the pathfinding algorithm is an optimised Dijkstra
   * algorithm. Production nodes can not traversed to gain access to other nodes.
   */
  PathFindingResult pathFind(unsigned origin, const node::NodeType& destination) const;

  /** Returns the node that intersects with the given piece of space, or NO_LINK */
  unsigned getNodeAt(tools::Vec2 position);

  /** Returns the uid of the selected node, or NO_LINK */
  unsigned getSelectedNode() const;
  /** Marks the node as selected, or NO_LINK to deselect the currently selected node */
  void selectNode(unsigned node);

  /** Whether to highlight the selected node's shortest paths when rendering */
  void setHighlightShortestPath(bool highlight);

  /** Marks the given nodes as highlighted */
  void highlightNodes(const std::vector<unsigned>& nodes, bool deselectOthers);
  /** Marks all nodes as not highlighted */
  void clearHighlightedNodes();

  /** Returns an available uid value */
  unsigned availableUid() const;

 private:
  /* Attributes */

  /** A uid-sorted map of nodes belonging to the town */
  std::map<unsigned, node::Node> nodes;

  /** A list of Link instances that are part of the town */
  std::vector<node::Link> links;

  /** The selected node, or NO_LINK if no node is selected */
  unsigned selectedNode;

  /**
   * Whether to highlight the shortest path from the selected node to a transport
   * and production node.
  */
  bool highlightShortestPath;

  /* Methods */

  /** Checks whether the given node intersects any town links */
  void checkNodeSuperposition(const node::Node& node,
                              const double safetyDistance = DEFAULT_SAFETY);

  /**
   * Checks whether the given node intersects any town links.
   * Does not require the node to be a part of the town.
   */
  void checkLinkSuperposition(const node::Node& node,
                              const double safetyDistance = DEFAULT_SAFETY);
  /** Checks whether the given link intersects any town nodes. */
  void checkLinkSuperposition(const node::Link& link,
                              const double safetyDistance = DEFAULT_SAFETY);
};

/* === FUNCTIONS === */

/** Read the given file and parse the town */
Town loadFromFile(const std::string& path);

/** Save the given town to a file */
void saveToFile(const std::string& path, const Town& town);

}  // namespace town

#endif
