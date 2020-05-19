// archipelago v2.0.0 - architecture b2
// town.cpp - town classes and functions
// Authors: Marcus Cemes, Alexandre Dodens

#include "town.hpp"

#include <algorithm>  // reverse()
#include <array>      // inline for loop
#include <cctype>     // isspace()
#include <fstream>    // istream
#include <iostream>   // cerr
#include <limits>     // numeric_limits
#include <map>        // validation
#include <memory>     // unique_ptr
#include <set>        // validation
#include <sstream>    // stringstream
#include <string>
#include <vector>

#include "constants.hpp"
#include "error.hpp"
#include "node.hpp"
#include "tools.hpp"

/* Select a few reused imports to help alleviate the syntax */
using node::Link;
using node::Node;
using node::NodeType;
using std::istream;
using std::map;
using std::ostream;
using std::set;
using std::string;
using std::unique_ptr;
using std::vector;
using tools::Vec2;
using town::Town;

/* === INTERNAL DEFINITIONS AND PROTOTYPES === */

namespace {

constexpr char COMMENT_DELIMITER('#');
constexpr int NB_LINK_UIDS(2);   // number of UIDs in a Link
constexpr double ZERO_TIME(0.);  // an absence of time
constexpr int LINE_START(0);     // beginning of a line

typedef vector<Node> Nodes;
typedef vector<Link> Links;

/** Represents a node in the Dijkstra algorithm graph */
struct DijkstraNode {
  bool visited;
  double distance;
  unsigned parent;
};

/** A graph of UIDs and nodes */
typedef map<unsigned, DijkstraNode> DijkstraGraph;

Town parseTown(istream& stream);
void parseNodes(istream& stream, Nodes& nodes, NodeType type);
void parseLinks(istream& stream, Links& links);

std::stringstream getNextLine(istream& stream);
unsigned readUnsigned(istream& stream);
unsigned long long readLongUnsigned(istream& stream);
double readDouble(istream& stream);

void writeTown(ostream& stream, const Town& town);

void printNodeType(ostream& stream, const Town& town, const NodeType& type);
void printLinks(ostream& stream, const Town& town);

DijkstraGraph createDijkstraGraph(const vector<unsigned>& uids, unsigned originUid);
double computeAccessTime(const NodeType& type0, const NodeType& type2,
                         double distance);
town::PathFindingResult generatePathResult(DijkstraGraph graph,
                                           unsigned destinationUid, double distance);
bool nextGraphNode(DijkstraGraph graph, unsigned& nextUid);
}  // namespace

namespace town {

/* === CLASSES === */

Town::Town(Nodes nodes, Links links) : selectedNode(NO_LINK) {
  for (const auto& node : nodes) {
    addNode(node);
  }
  for (const auto& link : links) {
    addLink(link);
  }
}

void Town::render(tools::RenderContext& ctx) {
  set<unsigned> tPathNodes, pPathNodes;

  // Path finding calculations
  clearHighlightedNodes();
  if (highlightShortestPath && selectedNode != NO_LINK &&
      getNode(selectedNode)->getType() == node::HOUSING) {
    const auto tPath(pathFind(selectedNode, node::TRANSPORT));
    if (tPath.success) {
      highlightNodes(*tPath.path, false);
      for (const auto& uid : *tPath.path) tPathNodes.insert(uid);
    }

    const auto pPath(pathFind(selectedNode, node::PRODUCTION));
    if (pPath.success) {
      highlightNodes(*pPath.path, false);
      for (const auto& uid : *pPath.path) pPathNodes.insert(uid);
    }
  }

  // Render links, highlight if they are in one of the path finding chains
  for (const auto& link : links) {
    const auto uid0((link.getUid0()));
    const auto uid1((link.getUid1()));
    bool highlighted(false);

    if ((tPathNodes.find(uid0) != tPathNodes.end() &&
         tPathNodes.find(uid1) != tPathNodes.end()) ||
        (pPathNodes.find(uid0) != pPathNodes.end() &&
         pPathNodes.find(uid1) != pPathNodes.end()))
      highlighted = true;

    ctx.setColour(highlighted ? tools::GREEN : tools::BLACK);
    ctx.draw(tools::Line(getNode(uid0)->getPosition(), getNode(uid1)->getPosition()));
  }

  // Render nodes, they know if they are highlighted
  for (auto& node : nodes) {
    node.second.render(ctx);
  }
}

void Town::addNode(const Node& node, const double safetyDistance) {
  const unsigned uid(node.getUid());

  // Check if the node already is part of the town
  if (nodes.count(uid) == 1) throw error::identical_uid(node.getUid());

  // Check if the new node would cause a superposition
  checkNodeSuperposition(node, safetyDistance);
  checkLinkSuperposition(node, safetyDistance);

  nodes.emplace(uid, node);  // avoid unnecessary copies
}

const Node* Town::getNode(const unsigned uid) const {
  auto node(nodes.find(uid));

  if (node == nodes.end()) return nullptr;
  return &(node->second);
}

Node* Town::getModifiableNode(const unsigned uid) {
  auto node(nodes.find(uid));

  if (node == nodes.end()) return nullptr;
  return &(node->second);
}

vector<unsigned> Town::getNodes() const {
  vector<unsigned> nodeUids;
  nodeUids.reserve(nodes.size());
  for (auto const& node : nodes) {
    nodeUids.push_back(node.second.getUid());
  }
  return nodeUids;
}

void Town::removeNode(unsigned uid) {
  // Efficiently delete links containing this node's uid
  for (auto it(links.begin()); it != links.end(); ++it)
    if (it->getUid0() == uid || it->getUid1() == uid) {
      *it = std::move(links.back());
      links.pop_back();
      --it;
    }

  if (selectedNode == uid) selectedNode = NO_LINK;
  nodes.erase(uid);
}

void Town::moveNode(unsigned uid, const tools::Vec2& newPosition) {
  auto node(nodes.find(uid));
  if (node == nodes.end()) return;
  tools::Vec2 oldPosition(node->second.getPosition());

  try {
    node->second.setPosition(newPosition);
    checkNodeSuperposition(node->second, DIST_MIN);
    checkLinkSuperposition(node->second, DIST_MIN);
    for (const auto& link : links)
      if (link.getUid0() == uid || link.getUid1() == uid)
        checkLinkSuperposition(link, DIST_MIN);

  } catch (std::string err) {
    node->second.setPosition(oldPosition);
    throw err;
  }
}

void Town::resizeNode(unsigned uid, unsigned newRadius) {
  auto node(nodes.find(uid));
  if (node != nodes.end()) {
    const unsigned oldCapacity(node->second.getCapacity());
    try {
      node->second.setRadius(newRadius);
      checkNodeSuperposition(node->second, DIST_MIN);
      checkLinkSuperposition(node->second, DIST_MIN);
    } catch (std::string& err) {
      node->second.setCapacity(oldCapacity);
      throw err;
    }
  }
}

void Town::addLink(const Link& link) {
  // Check that the link doesn't already exist
  for (const auto& townLink : links) {
    if (townLink == link)
      throw error::multiple_same_link(link.getUid0(), link.getUid1());
  }

  // Check that the nodes exist
  if (nodes.count(link.getUid0()) == 0) {
    throw error::link_vacuum(link.getUid0());
  } else if (nodes.count(link.getUid1()) == 0) {
    throw error::link_vacuum(link.getUid1());
  }

  // Check that the link would not exceed the housing limit
  std::array<unsigned, NB_LINK_UIDS> uids{link.getUid0(), link.getUid1()};
  for (const unsigned& uid : uids) {
    if (getNode(uid)->getType() == node::HOUSING) {
      if (getLinkedNodes(uid).size() >= MAX_LINK) throw error::max_link(uid);
    }
  }

  checkLinkSuperposition(link);

  links.push_back(link);
}

bool Town::hasLink(const Link& link) const {
  for (const auto& townLink : links) {
    if (link == townLink) return true;
  }
  return false;
}

const vector<Link>* Town::getLinks() const { return &links; }

vector<unsigned> Town::getLinkedNodes(const unsigned uid) const {
  if (nodes.count(uid) == 0) throw error::link_vacuum;

  vector<unsigned> nodeLinks;
  for (const auto& link : links) {
    if (link.getUid0() == uid) {
      nodeLinks.push_back(link.getUid1());
    } else if (link.getUid1() == uid) {
      nodeLinks.push_back(link.getUid0());
    }
  }
  return nodeLinks;
}

void Town::removeLink(const Link& link) {
  auto end(links.end());
  for (auto it(links.begin()); it < end; ++it) {
    if (link == *it) {
      links.erase(it);
      return;
    }
  }
}

double Town::enj() {
  double enjSum(0);
  unsigned population(0);

  for (const auto& node : nodes) {
    unsigned capacity(node.second.getCapacity());
    population += capacity;
    switch (node.second.getType()) {
      case node::HOUSING:
        enjSum += capacity;
        break;
      case node::TRANSPORT:
        enjSum -= capacity;
        break;
      case node::PRODUCTION:
        enjSum -= capacity;
        break;
    }
  }

  if (population == 0) return 0;  // special case
  return enjSum / population;
}

double Town::ci() {
  double ci(0);

  for (const auto& link : links) {
    auto node0(getNode(link.getUid0()));
    auto node1(getNode(link.getUid1()));

    // Distance
    double cost((node1->getPosition() - node0->getPosition()).norm());

    // Capacity
    if (node0->getCapacity() <= node1->getCapacity()) {
      cost *= node0->getCapacity();
    } else {
      cost *= node1->getCapacity();
    }

    // Speed
    if (node0->getType() == node::TRANSPORT && node1->getType() == node::TRANSPORT) {
      cost *= FAST_SPEED;
    } else {
      cost *= DEFAULT_SPEED;
    }

    ci += cost;
  }

  return ci;
}

double Town::mta() {
  double sum(0);
  double nbNodes(0);

  for (const auto& node : nodes) {
    if (node.second.getType() == node::HOUSING) {
      sum += pathFind(node.first, node::TRANSPORT).distance;
      sum += pathFind(node.first, node::PRODUCTION).distance;
      ++nbNodes;
    }
  }

  if (nbNodes == 0) return 0;  // special case
  return sum / nbNodes;
}

/**
 * Our only function that exceeds 40 lines, favourable for optimisation and spacing
 * An fast implementation of the Dijkstra algorithm.
 */
town::PathFindingResult Town::pathFind(unsigned originUid,
                                       const NodeType& searchType) const {
  if (getNode(originUid) == nullptr) throw string("Node does not exist");

  // Prepare algorithm variables
  DijkstraGraph graph(createDijkstraGraph(getNodes(), originUid));

  unsigned currentUid(originUid);
  const Node* currentNode(nullptr);
  const Node* neighbourNode(nullptr);

  DijkstraGraph::iterator currentGraphNode;  // TN(n)
  DijkstraGraph::iterator graphNeighbour;    // TN(lv)

  double currentDistance(0);    // "TN(n).access"
  double neighbourDistance(0);  // "alt" variable

  NodeType currentType;
  NodeType neighbourType;

  // Execute the algorithm
  while (nextGraphNode(graph, currentUid)) {
    currentNode = &nodes.find(currentUid)->second;
    currentGraphNode = graph.find(currentUid);
    currentDistance = currentGraphNode->second.distance;
    currentType = currentNode->getType();

    // Evaluate all graph neighbours
    for (const auto& neighbourUid : getLinkedNodes(currentGraphNode->first)) {
      graphNeighbour = graph.find(neighbourUid);
      if (graphNeighbour->second.visited) continue;

      neighbourNode = getNode(neighbourUid);
      neighbourNode = getNode(neighbourUid);
      neighbourType = neighbourNode->getType();
      neighbourDistance =
          (neighbourNode->getPosition() - currentNode->getPosition()).norm();

      neighbourDistance =
          currentDistance +
          computeAccessTime(currentType, neighbourType, neighbourDistance);

      if (neighbourDistance < graphNeighbour->second.distance) {
        graphNeighbour->second.distance = neighbourDistance;
        graphNeighbour->second.parent = currentUid;
      }

      // Mark production nodes as visited to deny through-access to other nodes
      if (neighbourType == node::PRODUCTION) graphNeighbour->second.visited = true;

      // Return condition: verify the destination condition has been met
      if (neighbourType == searchType)
        return generatePathResult(graph, neighbourUid, neighbourDistance);
    }

    // Mark the node as visited to progress the algorithm
    currentGraphNode->second.visited = true;
  }

  return generatePathResult(graph, NO_LINK, INFINITE_TIME);
}

unsigned Town::getNodeAt(tools::Vec2 position) {
  for (const auto& node : nodes)
    if ((node.second.getPosition() - position).norm() <= node.second.radius())
      return node.second.getUid();

  return NO_LINK;
}

unsigned Town::getSelectedNode() const { return selectedNode; }

void Town::selectNode(unsigned nodeToSelect) {
  // Deselect the currently selected node
  if (selectedNode != NO_LINK) {
    auto node(getModifiableNode(selectedNode));
    if (node != nullptr) node->setSelected(false);
  }

  // Select the new active node
  selectedNode = nodeToSelect;
  if (nodeToSelect != NO_LINK) {
    auto node(getModifiableNode(nodeToSelect));
    if (node != nullptr) {
      node->setSelected(true);
    }
  }
}

void Town::setHighlightShortestPath(bool highlight) {
  highlightShortestPath = highlight;
}

void Town::highlightNodes(const std::vector<unsigned>& highlighted, bool deselect) {
  if (deselect) clearHighlightedNodes();

  for (const auto& uid : highlighted) {
    auto node(getModifiableNode(uid));
    if (node != nullptr) node->setHighlighted(true);
  }
}

void Town::clearHighlightedNodes() {
  for (auto& node : nodes) node.second.setHighlighted(false);
}

unsigned Town::availableUid() const {
  for (size_t i(0); i <= NO_LINK; ++i)
    if (nodes.find(i) == nodes.end()) return i;

  return NO_LINK;
}

/* == Private members == */

/** Checks whether the given node intersects any town links */
void Town::checkLinkSuperposition(const Node& testNode, const double safetyDistance) {
  unsigned uid(testNode.getUid()), link0, link1;
  double radius;
  for (const auto& townLink : links) {
    link0 = townLink.getUid0();
    link1 = townLink.getUid1();

    // Ignore node connections to self, these can violate safety distances
    if (uid == link0 || uid == link1) continue;
    radius = testNode.radius();

    if (minPointSegmentDistance(testNode.getPosition(), nodes.at(link0).getPosition(),
                                nodes.at(link1).getPosition()) <=
        (radius + safetyDistance)) {
      throw error::node_link_superposition(uid);
    }
  }
}

/** Checks whether the given link would intersect any town nodes */
void Town::checkLinkSuperposition(const Link& testLink, const double safetyDistance) {
  double radius;
  unsigned link0(testLink.getUid0()), link1(testLink.getUid1());

  // Assumes that node existance was already checked
  Vec2 link0Pos(getNode(link0)->getPosition());
  Vec2 link1Pos(getNode(link1)->getPosition());

  for (const auto& townNode : nodes) {
    unsigned uid(townNode.second.getUid());

    // Ignore node connections to self, these can violate safety distances
    if (uid == link0 || uid == link1) continue;
    radius = townNode.second.radius();

    if (minPointSegmentDistance(townNode.second.getPosition(), link0Pos, link1Pos) <=
        (radius + safetyDistance)) {
      throw error::node_link_superposition(uid);
    }
  }
}

/** Checks whether the given node would intersect any town nodes */
void Town::checkNodeSuperposition(const Node& testNode, const double safetyDistance) {
  double distance;
  for (const auto& townNodePair : nodes) {
    const Node* townNode(&townNodePair.second);
    if (testNode.getUid() == townNode->getUid()) continue;

    distance = (testNode.getPosition() - townNode->getPosition()).norm();
    if (distance <= testNode.radius() + townNode->radius() + safetyDistance) {
      throw error::node_node_superposition(testNode.getUid(), townNode->getUid());
    }
  }
}

/* === FUNCTIONS === */

Town loadFromFile(const string& path) {
  std::ifstream file(path);
  if (file.is_open()) {
    return Town(parseTown(file));
  } else {
    std::cerr << "Error: Could not open file" << std::endl;
    return Town();
  }
}

void saveToFile(const std::string& path, const Town& town) {
  std::ofstream file(path);
  if (file.is_open()) {
    writeTown(file, town);
    file.close();
  } else {
    std::cerr << "Error: Could not open file" << std::endl;
  }
}

}  // namespace town

/* === INTERNAL FUNCTIONS === */

namespace {

/* == Town parsing == */

/**
 * Reads an entire input stream and generates a town using the archipelago file
 * format.
 *
 * If quitOnError is true, the program will output the results to the terminal,
 * and additionally exit immediately if a parsing error is encountered.
 */
Town parseTown(istream& stream) {
  Nodes nodes;
  Links links;

  // Parse each node
  parseNodes(stream, nodes, node::HOUSING);
  parseNodes(stream, nodes, node::TRANSPORT);
  parseNodes(stream, nodes, node::PRODUCTION);

  // Parse each link
  parseLinks(stream, links);

  // Construct the town and return
  Town town(nodes, links);

  return town;
}

/**
 * Read and parse a single node type from an input stream, create the Node
 * instances and append them to the given vector. This function initially reads
 * the node count.
 */
void parseNodes(istream& rawStream, Nodes& nodes, NodeType type) {
  std::stringstream lineStream(getNextLine(rawStream));

  size_t count(readLongUnsigned(lineStream));

  unsigned int uid, capacity;
  double x, y;

  // Read as many nodes as were specified by the count
  for (size_t i(0); i < count; ++i) {
    lineStream = getNextLine(rawStream);
    uid = readUnsigned(lineStream);
    x = readDouble(lineStream);
    y = readDouble(lineStream);
    capacity = readUnsigned(lineStream);

    nodes.push_back(Node(type, uid, {x, y}, capacity));
  }
}

/**
 * Read and parse links from an input stream, creating Link objects and
 * appending them to a vector.
 */
void parseLinks(istream& rawStream, Links& links) {
  std::stringstream lineStream(getNextLine(rawStream));

  size_t count(readLongUnsigned(lineStream));
  unsigned int uid0, uid1;

  // Read as many links as were specified by the count
  for (size_t i(0); i < count; ++i) {
    lineStream = getNextLine(rawStream);
    uid0 = readUnsigned(lineStream);
    uid1 = readUnsigned(lineStream);

    links.push_back({uid0, uid1});
  }
}

/**
 * Read a single line of real content (containing readable characters) from an
 * input stream. Each line is stripped of comments before being returned.
 */
std::stringstream getNextLine(istream& stream) {
  // Signal the end by return a stringstream with an EOF bit
  if (stream.eof()) {
    std::stringstream emptyStream("");
    emptyStream.ignore(std::numeric_limits<std::streamsize>::max());
    return emptyStream;
  }

  string line;
  std::getline(stream, line);

  // Trim off comments
  size_t commentPos(line.find(COMMENT_DELIMITER));
  if (commentPos != string::npos) {
    line = line.substr(LINE_START, commentPos);
  }

  // search for non-whitespace character
  bool hasContent(false);
  for (const char& stringChar : line) {
    if (!isspace(stringChar)) {
      hasContent = true;
      break;
    }
  }

  // skip empty lines
  if (!hasContent) {
    return getNextLine(stream);  // recursively fetch the next line
  }

  std::stringstream lineStream(line);
  return lineStream;
}

/** Read and return an int from an input stream */
unsigned readUnsigned(istream& stream) {
  unsigned buffer(0);
  stream >> buffer;
  return buffer;
}

/** Read and return an int from an input stream */
unsigned long long readLongUnsigned(istream& stream) {
  unsigned long long buffer(0);
  stream >> buffer;
  return buffer;
}

/** Read and return a double from an input stream */
double readDouble(istream& stream) {
  double buffer(0.);
  stream >> buffer;
  return buffer;
}

/* == Saving == */

/** Serialises the town into a streamable format */
void writeTown(ostream& stream, const Town& town) {
  stream << COMMENT_DELIMITER << " Archipelago Town" << std::endl;
  stream << COMMENT_DELIMITER << " AUTOMATICALLY GENERATED FILE" << std::endl;

  printNodeType(stream, town, node::HOUSING);
  printNodeType(stream, town, node::TRANSPORT);
  printNodeType(stream, town, node::PRODUCTION);

  printLinks(stream, town);
}

void printNodeType(ostream& stream, const Town& town, const NodeType& type) {
  unsigned count(0);
  vector<unsigned> uids;
  Vec2 position;

  stream << std::endl;

  // Find all nodes of a certain type
  for (const auto& uid : town.getNodes()) {
    if (town.getNode(uid)->getType() == type) {
      ++count;
      uids.push_back(uid);
    }
  }

  stream << count << std::endl;

  for (const auto& uid : uids) {
    stream << town.getNode(uid)->toString() << std::endl;
  }
}

void printLinks(ostream& stream, const Town& town) {
  auto links(town.getLinks());

  stream << std::endl << links->size() << std::endl;
  for (const auto& link : *links) {
    stream << link.getUid0() << " " << link.getUid1() << std::endl;
  }
}

/* == Dijkstra == */

/**
 * Creates a graph that is suitable for path finding calculations using the
 * Dijkstra algorithm. All nodes will univisited and set to INFINITE_TIME, apart
 * from the origin node which will have a distance of 0.
 */
DijkstraGraph createDijkstraGraph(const vector<unsigned>& uids, unsigned originUid) {
  DijkstraGraph graph;
  for (const auto& uid : uids) {
    if (uid == originUid) {
      graph.insert({uid, {false, ZERO_TIME, NO_LINK}});
    } else {
      graph.insert({uid, {false, INFINITE_TIME, NO_LINK}});
    }
  }

  return graph;
}

/** Computes the access time between two nodes */
double computeAccessTime(const NodeType& type0, const NodeType& type1,
                         double distance) {
  if (type0 == node::TRANSPORT && type1 == node::TRANSPORT)
    return distance / FAST_SPEED;
  return distance / DEFAULT_SPEED;
}

/** Generates a result of a path finding operation, returning whether a
 * destination was found, a list of UIDs in the path and the total distance */
town::PathFindingResult generatePathResult(DijkstraGraph graph,
                                           unsigned destinationUid, double distance) {
  bool success(destinationUid != NO_LINK);
  town::Path path;

  if (success) {
    path.reset(new vector<unsigned>());
    path->push_back(destinationUid);
    auto graphNode(graph.find(destinationUid));

    while (graphNode->second.parent != NO_LINK) {
      graphNode = graph.find(graphNode->second.parent);
      path->push_back(graphNode->first);
    }
    std::reverse(path->begin(), path->end());
  }

  return {success, std::move(path), distance};
}

/** Fins the next node, and sets nextUid. Returns a bool whether a node was found */
bool nextGraphNode(DijkstraGraph graph, unsigned& nextUid) {
  unsigned closestUid(NO_LINK);
  double closestDistance(INFINITE_TIME);

  for (const auto& node : graph) {
    if (!node.second.visited && node.second.distance < closestDistance) {
      closestUid = node.first;
      closestDistance = node.second.distance;
    }
  }

  nextUid = closestUid;
  return closestUid != NO_LINK;
}

}  // namespace
