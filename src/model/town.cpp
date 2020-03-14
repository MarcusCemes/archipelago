// archipelago - model/town.cpp
// Town-related logic, such as ENJ, CI and MTA calculations

#include <fstream>   // istream
#include <iostream>  // cerr
#include <limits>    // numeric_mitis
#include <sstream>   // stringstream
#include <string>
#include <vector>

#include "node.hpp"
#include "town.hpp"

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
int readInt(std::istream &stream);
double readDouble(std::istream &stream);
}  // namespace

/* === CLASSES === */

town::Town::Town(Nodes nodes, Links links) : nodes(nodes), links(links) {}

Nodes town::Town::getNodes() { return nodes; }
void town::Town::setNodes(Nodes newNodes) { nodes = newNodes; }

Links town::Town::getLinks() { return links; }
void town::Town::setLinks(Links newLinks) { links = newLinks; }

/** Count number of links connected to a node */
unsigned int town::Town::get_links(node::Node node) {
  unsigned int number_links(0);
  for (size_t i(0); i < links.size(); ++i) {
    if ((links[i].uid0 == node.getUid()) || (links[i].uid1 == node.getUid())) {
      ++number_links;
    }
  }

  return number_links;
}

/* === FUNCTIONS === */

/** Load and parse a town from a file. Returns an empty town on failure */
town::Town town::loadFromFile(char *path) {
  std::ifstream file(path);
  if (file.is_open()) {
    town::Town town(parseTown(file));
    return town;
  } else {
    std::cerr << "Error: Could not open file" << std::endl;
    return town::Town();
  }
}

/* === INTERNAL FUNCTIONS === */

namespace {
/** Parses a town from a multiline input stream */
town::Town parseTown(std::istream &stream) {
  Nodes nodes;
  Links links;

  // Parse each node
  node::NodeType types[] = {node::HOUSING, node::TRANSPORT, node::PRODUCTION};
  for (auto &type : types) {
    parseNodes(stream, nodes, type);
  }

  // Parse each link
  parseLinks(stream, links);

  town::Town createdTown(nodes, links);
  return createdTown;
}

/** Read a certain node type and apprend to a vector */
void parseNodes(std::istream &rawStream, Nodes &nodes, node::NodeType type) {
  std::stringstream lineStream(getNextLine(rawStream));

  size_t count(readInt(lineStream));

  unsigned int uid, capacity;
  double x, y;

  // Read as many nodes as were specified by the count
  for (size_t i(0); i < count; ++i) {
    lineStream = getNextLine(rawStream);
    uid = readInt(lineStream);
    x = readDouble(lineStream);
    y = readDouble(lineStream);
    capacity = readInt(lineStream);

    nodes.push_back(node::Node(type, uid, {x, y}, capacity));
  }
}

void parseLinks(std::istream &rawStream, Links &links) {
  std::stringstream lineStream(getNextLine(rawStream));

  size_t count(readInt(lineStream));
  unsigned int uid0, uid1;

  // Read as many links as were specified by the count
  for (size_t i(0); i < count; ++i) {
    lineStream = getNextLine(rawStream);
    uid0 = readInt(lineStream);
    uid1 = readInt(lineStream);

    links.push_back({uid0, uid1});
  }
}

/**
 * Read a single line from a stream. Ignores comment lines.
 */
std::stringstream getNextLine(std::istream &stream) {
  // Return a stringstream with an EOF bit
  if (stream.eof()) {
    std::stringstream emptyStream("");
    emptyStream.ignore(std::numeric_limits<std::streamsize>::max());
    return emptyStream;
  }

  std::string line;
  std::getline(stream, line);

  bool hasComment(line.find(COMMENT_DELIMITER) != std::string::npos);
  bool hasContent(line.length() != 0 &&
                  line.find_first_not_of(SPACE_CHAR) != std::string::npos);

  // skip comments or empty lines
  if (hasComment || !hasContent) {
    return getNextLine(stream);  // recursively fetch the next line
  }

  std::stringstream lineStream(line);
  return lineStream;
}

int readInt(std::istream &stream) {
  int buffer(0);
  stream >> buffer;
  return buffer;
}

double readDouble(std::istream &stream) {
  double buffer(0.);
  stream >> buffer;
  return buffer;
}

}  // namespace
