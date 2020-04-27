// archipelago - model/town.cpp
// Town-related logic, such as ENJ, CI and MTA calculations

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
using std::map;
using std::set;
using std::string;
using std::unique_ptr;
using std::vector;
using tools::Vec2;
using town::Town;

/* === INTERNAL DEFINITIONS AND PROTOTYPES === */

namespace {

constexpr char COMMENT_DELIMITER('#');
constexpr unsigned ERROR_EXIT_CODE(1);
constexpr int NB_LINK_UIDS(2);
constexpr double ZERO_TIME(0);

typedef vector<Node> Nodes;
typedef vector<Link> Links;

/** Represents a node in the Dijkstra algorithm graph */
struct DijkstraNode {
  bool visited;
  double distance;
  unsigned parent;
};

/** A graph of UIDs and nodes */
typedef std::map<unsigned, DijkstraNode> DijkstraGraph;

Town parseTown(std::istream& stream, bool quitOnError = false);
void parseNodes(std::istream& stream, Nodes& nodes, node::NodeType type);
void parseLinks(std::istream& stream, Links& links);

std::stringstream getNextLine(std::istream& stream);
unsigned readUnsigned(std::istream& stream);
unsigned long long readLongUnsigned(std::istream& stream);
double readDouble(std::istream& stream);

DijkstraGraph createDijkstraGraph(const std::vector<unsigned>& uids,
                                  unsigned originUid);
double computeAccessTime(const node::NodeType& type0,
                         const node::NodeType& type2, double distance);
town::PathFindingResult generatePathResult(DijkstraGraph graph,
                                           unsigned destinationUid,
                                           double distance);
unsigned closestUnvisited(DijkstraGraph graph);
}  // namespace

namespace town {

/* === CLASSES === */

Town::Town(Nodes nodes, Links links) {
  for (const auto& node : nodes) {
    addNode(node);
  }
  for (const auto& link : links) {
    addLink(link);
  }
}

void Town::render(tools::RenderContext& ctx) const {
  for (const auto& link : links) {
    ctx.draw(tools::Line(getNode(link.getUid0())->getPosition(),
                         getNode(link.getUid1())->getPosition()));
  }
  for (const auto& node : nodes) {
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

void Town::removeNode(const unsigned uid) { nodes.erase(uid); }

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
  double enjSum(0.);
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

  if (population == 0) return 0.;
  return enjSum / population;
}

double Town::ci() {
  double ci(0.);

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
    if (node0->getType() == node::TRANSPORT &&
        node1->getType() == node::TRANSPORT) {
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

  if (nbNodes == 0) return 0.;
  return sum / nbNodes;
}

town::PathFindingResult Town::pathFind(unsigned originUid,
                                       const node::NodeType& searchType) const {
  if (getNode(originUid) == nullptr) throw std::string("Node does not exist");

  // Prepare algorithm variables
  DijkstraGraph graph(createDijkstraGraph(getNodes(), originUid));

  unsigned currentUid(originUid);
  const Node* currentNode(nullptr);
  const Node* neighbourNode(nullptr);

  auto currentGraphNode(graph.find(originUid));  // TN(n)
  DijkstraGraph::iterator graphNeighbour;        // TN(lv)

  double currentDistance(0);    // "TN(n).access"
  double neighbourDistance(0);  // "alt" variable

  node::NodeType currentType;
  node::NodeType neighbourType;

  // Execute the algorithm
  while (true) {
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
      if (neighbourType == node::PRODUCTION)
        graphNeighbour->second.visited = true;

      // Return condition: verify the destination condition has been met
      if (neighbourType == searchType)
        return generatePathResult(graph, neighbourUid, neighbourDistance);
    }

    // Break condition: no more non-visited nodes
    currentGraphNode->second.visited = true;
    currentUid =
        closestUnvisited(graph);  // prioritse nodes with lowest access time
    if (currentUid == NO_LINK) break;
  }

  return generatePathResult(graph, NO_LINK, INFINITE_TIME);
}

/* == Private members == */

/** Checks whether the given node intersects any town links */
void Town::checkLinkSuperposition(const Node& testNode,
                                  const double safetyDistance) {
  unsigned uid(testNode.getUid()), link0, link1;
  double radius;
  for (const auto& townLink : links) {
    link0 = townLink.getUid0();
    link1 = townLink.getUid1();

    // Ignore node connections to self, these can violate safety distances
    if (uid == link0 || uid == link1) continue;
    radius = testNode.radius();

    if (minPointSegmentDistance(
            nodes.at(uid).getPosition(), nodes.at(link0).getPosition(),
            nodes.at(link1).getPosition()) <= (radius + safetyDistance)) {
      throw error::node_link_superposition(uid);
    }
  }
}

/** Checks whether the given link would intersect any town nodes */
void Town::checkLinkSuperposition(const Link& testLink,
                                  const double safetyDistance) {
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

    if (minPointSegmentDistance(townNode.second.getPosition(), link0Pos,
                                link1Pos) <= (radius + safetyDistance)) {
      throw error::node_link_superposition(uid);
    }
  }
}

/** Checks whether the given node would intersect any town nodes */
void Town::checkNodeSuperposition(const Node& testNode,
                                  const double safetyDistance) {
  double distance;
  for (const auto& townNodePair : nodes) {
    const Node* townNode(&townNodePair.second);

    distance = (testNode.getPosition() - townNode->getPosition()).norm();
    if (distance <= testNode.radius() + townNode->radius() + safetyDistance) {
      throw error::node_node_superposition(testNode.getUid(),
                                           townNode->getUid());
    }
  }
}

/* === FUNCTIONS === */

/** Load a town from a file, or create a new one if file does not exist */
Town loadFromFile(const std::string& path) {
  std::ifstream file(path);
  if (file.is_open()) {
    return Town(parseTown(file, false));  // TODO remove quitting
  } else {
    std::cerr << "Error: Could not open file" << std::endl;
    return Town();
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
Town parseTown(std::istream& stream, bool quitOnError) {
  Nodes nodes;
  Links links;

  try {
    // Parse each node
    parseNodes(stream, nodes, node::HOUSING);
    parseNodes(stream, nodes, node::TRANSPORT);
    parseNodes(stream, nodes, node::PRODUCTION);

    // Parse each link
    parseLinks(stream, links);

    // Construct the town and return
    Town town(nodes, links);

    if (quitOnError) std::cout << error::success();

    return town;

  } catch (string& error) {
    if (quitOnError == true) {
      std::cout << error;
      exit(ERROR_EXIT_CODE);
    } else {
      throw error;
    }
  }

  return Town();
}

/**
 * Read and parse a single node type from an input stream, create the Node
 * instances and append them to the given vector. This function initially reads
 * the node count.
 */
void parseNodes(std::istream& rawStream, Nodes& nodes, node::NodeType type) {
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
void parseLinks(std::istream& rawStream, Links& links) {
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
std::stringstream getNextLine(std::istream& stream) {
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
    line = line.substr(0, commentPos);
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
unsigned readUnsigned(std::istream& stream) {
  unsigned buffer(0);
  stream >> buffer;
  return buffer;
}

/** Read and return an int from an input stream */
unsigned long long readLongUnsigned(std::istream& stream) {
  unsigned long long buffer(0);
  stream >> buffer;
  return buffer;
}

/** Read and return a double from an input stream */
double readDouble(std::istream& stream) {
  double buffer(0.);
  stream >> buffer;
  return buffer;
}

/* == Dijkstra == */

/** Creates a graph that is suitable for path finding calculations using the
 * Dijkstra algorithm */
DijkstraGraph createDijkstraGraph(const std::vector<unsigned>& uids,
                                  unsigned originUid) {
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
double computeAccessTime(const node::NodeType& type0,
                         const node::NodeType& type1, double distance) {
  if (type0 == node::TRANSPORT && type1 == node::TRANSPORT)
    return distance / FAST_SPEED;
  return distance / DEFAULT_SPEED;
}

/** Generates a result of a path finding operation, returning whether a
 * destination was found, a list of UIDs in the path and the total distance */
town::PathFindingResult generatePathResult(DijkstraGraph graph,
                                           unsigned destinationUid,
                                           double distance) {
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

/** Returns the closest node in a Dijkstra graph */
unsigned closestUnvisited(DijkstraGraph graph) {
  unsigned closestUid(NO_LINK);
  double closestDistance(INFINITE_TIME);

  for (const auto& node : graph) {
    if (!node.second.visited && node.second.distance < closestDistance) {
      closestUid = node.first;
      closestDistance = node.second.distance;
    }
  }

  return closestUid;
}

}  // namespace
