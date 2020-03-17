// archipelago - model/town.cpp
// Town-related logic, such as ENJ, CI and MTA calculations

// #include <cmath>
#include "town.hpp"

#include <fstream>   // istream
#include <iostream>  // cerr
#include <limits>    // numeric_mitis
#include <map>       // validation
#include <set>       // validation
#include <sstream>   // stringstream
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
using std::vector;
using tools::Vec2;
using town::Town;

/* === INTERNAL DEFINITIONS AND PROTOTYPES === */

namespace {

constexpr char COMMENT_DELIMITER('#');
constexpr char SPACE_CHAR(' ');
constexpr char TAB_CHAR('\t');
constexpr char NULL_CHAR('\0');

typedef vector<Node> Nodes;
typedef vector<Link> Links;

/* Town (file) parsing */
Town parseTown(std::istream& stream);
void parseNodes(std::istream& stream, Nodes& nodes, node::NodeType type);
void parseLinks(std::istream& stream, Links& links);

std::stringstream getNextLine(std::istream& stream);
unsigned readUnsigned(std::istream& stream);
unsigned long long readLongUnsigned(std::istream& stream);
double readDouble(std::istream& stream);

/* Town validation */

string validateAll(const Nodes& nodes, const Links& Links);

void duplicateUids(const Nodes& nodes);
void linkUidsExist(const Nodes& nodes, const Links& links);
void duplicateLinks(const Links& links);
void nodeLinkCollision(const Nodes& nodes, const Links& links);
void nodeCollision(const Nodes& nodes);
void maxHousingConnections(const Links& links);
void reservedUid(const Nodes& nodes);
void badLink(const Links& links);
void validCapacity(const Nodes& nodes);

}  // namespace

namespace town {

/* === CLASSES === */

Town::Town(Nodes nodes, Links links) : nodes(nodes), links(links) {}

Nodes Town::getNodes() const { return nodes; }
void Town::setNodes(Nodes newNodes) { nodes = newNodes; }

Links Town::getLinks() const { return links; }
void Town::setLinks(Links newLinks) { links = newLinks; }

/* === FUNCTIONS === */

/** Load a town from a file, or create a new one if file does not exist */
Town loadFromFile(char* path) {
  std::ifstream file(path);
  if (file.is_open()) {
    Town town(parseTown(file));
    return town;
  } else {
    std::cerr << "Error: Could not open file" << std::endl;
    return Town();
  }
}

}  // namespace town

/* === INTERNAL FUNCTIONS === */

namespace {

/* == Town parsing == */

/** Parses a town from a multiline input stream */
Town parseTown(std::istream& stream) {
  Nodes nodes;
  Links links;

  // Parse each node
  parseNodes(stream, nodes, node::HOUSING);
  parseNodes(stream, nodes, node::TRANSPORT);
  parseNodes(stream, nodes, node::PRODUCTION);

  // Parse each link
  parseLinks(stream, links);

  // Validate all constraints on nodes and links
  //! For the first program version, this may cause program termination
  string result(validateAll(nodes, links));
  std::cout << result;
  if (result != error::success()) exit(1);

  Town createdTown(nodes, links);
  return createdTown;
}

/** Read a certain node type and apprend to a vector */
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

/** Read a single line from a stream. Ignores comment lines. */
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

  bool hasContent(line.length() != 0 &&
                  line.find_first_not_of(SPACE_CHAR) != string::npos);

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

/* == Town validation == */

/**
 * Node & Link validation. Returns the validation status string.
 * Unfortunately, despite issue #26490, the order *does* matter.
 */
string validateAll(const Nodes& nodes, const Links& links) {
  try {
    /** Independent tests - These must be validated first */
    reservedUid(nodes);
    duplicateUids(nodes);
    linkUidsExist(nodes, links);  // should come after `reservedUid`
    duplicateLinks(links);
    validCapacity(nodes);
    badLink(links);

    /* Dependent tests - These depend on the above */
    // e.g. An illegal capacity size may cause a "false" node collision
    nodeLinkCollision(nodes, links);
    nodeCollision(nodes);
    maxHousingConnections(links);  // This MUST execute after `duplcateLinks`
    return error::success();
  } catch (string& err) {
    return err;
  }
}

/** Iterate over a list of nodes and checkfor duplicate uids */
void duplicateUids(const Nodes& nodes) {
  auto END(nodes.end());
  for (auto i(nodes.begin()); i != END; ++i) {
    for (auto j(nodes.begin()); j != END; ++j) {
      if (j <= i) continue;

      unsigned iUid((*i).getUid()), jUid((*j).getUid());
      if (iUid == jUid) {
        throw error::identical_uid(iUid);
      }
    }
  }
}

/** Check that each link's nodes exist */
void linkUidsExist(const Nodes& nodes, const Links& links) {
  // Generate an optimised uid set
  vector<unsigned> listOfUids;
  listOfUids.reserve(nodes.size());
  for (const auto& node : nodes) {
    listOfUids.push_back(node.getUid());
  }
  set<unsigned> nodeUids(listOfUids.begin(), listOfUids.end());

  // Iterate over links and check for the existence of the node's uid
  for (const auto& link : links) {
    if (nodeUids.find(link.getUid0()) == nodeUids.end()) {
      throw error::link_vacuum(link.getUid0());
    }
    if (nodeUids.find(link.getUid1()) == nodeUids.end()) {
      throw error::link_vacuum(link.getUid1());
    }
  }
}

/** Check for duplicate links */
void duplicateLinks(const Links& links) {
  // Iterate over each pair of links, avoiding duplicate work
  auto END(links.end());
  for (auto i(links.begin()); i != END; ++i) {
    for (auto j(links.begin()); j != END; ++j) {
      if (j <= i) continue;

      // Make use of Link's `operator==` overloading
      if ((*i) == (*j)) {
        throw error::multiple_same_link((*i).getUid0(), (*i).getUid1());
      }
    }
  }
}

/** Check that all nodes have a allowed capacity */
void validCapacity(const Nodes& nodes) {
  unsigned capacity;
  for (const auto& node : nodes) {
    capacity = node.getCapacity();
    if (capacity < MIN_CAPACITY) throw error::too_little_capacity(capacity);
    if (capacity > MAX_CAPACITY) throw error::too_much_capacity(capacity);
  }
}

/** Checks for collision between nodes and links */
void nodeLinkCollision(const Nodes& nodes, const Links& links) {
  // Optimised map of node positions and sizes (pending better node lookup)
  map<unsigned, Vec2> positionMap;
  for (const auto& node : nodes) positionMap[node.getUid()] = node.getPosition();

  // Check for collisions
  unsigned uid, link0, link1;
  double radius;
  for (const auto& node : nodes) {
    for (const auto& link : links) {
      uid = node.getUid();
      link0 = link.getUid0();
      link1 = link.getUid1();

      // Ignore node connections to self, these can violate safety distances
      if (uid == link0 || uid == link1) continue;
      radius = node.radius();

      if (minPointSegmentDistance(positionMap[uid], positionMap[link0],
                                  positionMap[link1]) <= (radius + DIST_MIN)) {
        throw error::node_link_superposition(uid);
      }
    }
  }
}

/** Check for collision between nodes with a safety distance */
void nodeCollision(const Nodes& nodes) {
  double distance;
  auto END(nodes.end());
  for (auto i(nodes.begin()); i != END; ++i) {
    for (auto j(nodes.begin()); j != END; ++j) {
      if (j <= i) continue;

      distance = ((*j).getPosition() - (*i).getPosition()).norm();
      if (distance <= (*i).radius() + (*j).radius() + DIST_MIN) {
        throw error::node_node_superposition((*i).getUid(), (*j).getUid());
      }
    }
  }
}

/** Count connections for each node, and check it falls within regulation */
void maxHousingConnections(const Links& links) {
  // Create an optimised map of (uid, numberConnections) pairs
  map<unsigned, unsigned> connections;
  for (const auto& link : links) {
    // Cool C++ syntax, starts from zero if doesn't exist
    ++connections[link.getUid0()];
    ++connections[link.getUid1()];
  }

  // Count number of connections for each node
  for (const auto& connection : connections) {
    if (connection.second > MAX_LINK) throw error::max_link(connection.first);
  }
}

/** Check that no nodes are using a reserved uid */
void reservedUid(const Nodes& nodes) {
  for (const auto& node : nodes) {
    if (node.getUid() == NO_LINK) throw error::reserved_uid();
  }
}

/** Check that there are no links to the same uid */
void badLink(const Links& links) {
  for (const auto& link : links) {
    if (link.getUid0() == link.getUid1()) throw error::self_link_node(link.getUid1());
  }
}

}  // namespace
