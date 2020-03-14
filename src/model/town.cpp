// archipelago - model/town.cpp
// Town-related logic, such as ENJ, CI and MTA calculations

#include <fstream>   // istream
#include <iostream>  // cerr
#include <limits>    // numeric_mitis
#include <sstream>   // stringstream
#include <string>
#include <vector>

#include "error.hpp"
#include "node.hpp"
#include "town.hpp"
#include "validation.hpp"

/* === INTERNAL DEFINITIONS AND PROTOTYPES === */

namespace {

constexpr char COMMENT_DELIMITER('#');
constexpr char SPACE_CHAR(' ');
constexpr char TAB_CHAR('\t');
constexpr char NULL_CHAR('\0');

typedef std::vector<node::Node> Nodes;
typedef std::vector<node::Link> Links;

town::Town parseTown(std::istream &stream);
void parseNodes(std::istream &stream, Nodes &nodes, node::NodeType type);
void parseLinks(std::istream &stream, Links &links);

std::stringstream getNextLine(std::istream &stream);
unsigned readUnsigned(std::istream &stream);
unsigned long long readLongUnsigned(std::istream &stream);
double readDouble(std::istream &stream);
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
Town loadFromFile(char *path) {
  std::ifstream file(path);
  if (file.is_open()) {
    town::Town town(parseTown(file));
    return town;
  } else {
    std::cerr << "Error: Could not open file" << std::endl;
    return town::Town();
  }
}

}  // namespace town

/* === INTERNAL FUNCTIONS === */

namespace {
/** Parses a town from a multiline input stream */
town::Town parseTown(std::istream &stream) {
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
  std::string result(validation::validateAll(nodes, links));
  std::cout << result;
  if (result != error::success()) exit(1);

  town::Town createdTown(nodes, links);
  return createdTown;
}

/** Read a certain node type and apprend to a vector */
void parseNodes(std::istream &rawStream, Nodes &nodes, node::NodeType type) {
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

    nodes.push_back(node::Node(type, uid, {x, y}, capacity));
  }
}

void parseLinks(std::istream &rawStream, Links &links) {
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
std::stringstream getNextLine(std::istream &stream) {
  // Signal the end by return a stringstream with an EOF bit
  if (stream.eof()) {
    std::stringstream emptyStream("");
    emptyStream.ignore(std::numeric_limits<std::streamsize>::max());
    return emptyStream;
  }

  std::string line;
  std::getline(stream, line);

  // Trim off comments
  size_t commentPos(line.find(COMMENT_DELIMITER));
  if (commentPos != std::string::npos) {
    line = line.substr(0, commentPos);
  }

  bool hasContent(line.length() != 0 &&
                  line.find_first_not_of(SPACE_CHAR) != std::string::npos);

  // skip empty lines
  if (!hasContent) {
    return getNextLine(stream);  // recursively fetch the next line
  }

  std::stringstream lineStream(line);
  return lineStream;
}

/** Read and return an int from an input stream */
unsigned readUnsigned(std::istream &stream) {
  unsigned buffer(0);
  stream >> buffer;
  return buffer;
}

/** Read and return an int from an input stream */
unsigned long long readLongUnsigned(std::istream &stream) {
  unsigned long long buffer(0);
  stream >> buffer;
  return buffer;
}

/** Read and return a double from an input stream */
double readDouble(std::istream &stream) {
  double buffer(0.);
  stream >> buffer;
  return buffer;
}

}  // namespace
