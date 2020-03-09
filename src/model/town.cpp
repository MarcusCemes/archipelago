// archipelago - model/town.cpp
// Town-related logic, such as ENJ, CI and MTA calculations

#include <fstream>   // istream
#include <iostream>  // cerr
#include <sstream>   // stringstream
#include <string>
#include <vector>

#include "node.hpp"
#include "town.hpp"

/* === INTERNAL DEFINITIONS AND PROTOTYPES === */

namespace {
/** Last selected type to be created */
enum SelectedType { T_NULL, T_HOUSING, T_TRANSPORT, T_PRODUCTION, T_LINK };

constexpr char COMMENT_DELIMITER('#');
constexpr char NULL_CHAR('\0');

town::Town parseTown(std::istream &stream);
bool readLine(std::istream &stream, std::string &line);
void parseLine(std::string &line, std::vector<node::Node> &nodes,
               std::vector<node::Link> &links, SelectedType &selectedType);
void readWords(std::istream &stream, std::vector<std::string> &words);
void parseNode(std::vector<node::Node> &nodes, std::vector<std::string> &words,
               SelectedType &selectedType);
void parseLink(std::vector<node::Link> &links, std::vector<std::string> &words);
std::string extractValue(const std::string word);
int getFirstNumberIndex(const std::string word);
bool startsWith(const std::string sample, const std::string term);
}  // namespace

/* === CLASSES === */

town::Town::Town(std::vector<node::Node> nodes, std::vector<node::Link> links)
    : nodes(nodes), links(links) {}

std::vector<node::Node> town::Town::getNodes() { return nodes; }
void town::Town::setNodes(std::vector<node::Node> newNodes) {
  nodes = newNodes;
}

std::vector<node::Link> town::Town::getLinks() { return links; }
void town::Town::setLinks(std::vector<node::Link> newLinks) {
  links = newLinks;
}

/* === FUNCTIONS === */

/** Load and parse a town from a file. Returns an empty town on failure */
town::Town town::loadFromFile(char *path) {
  std::ifstream file(path);
  if (file.is_open()) {
    town::Town town(parseTown(file));
    return town;
  } else {
    std::cerr << "File not open" << std::endl;
    return town::Town();
  }
}

/** Count number of links connected dto a node */
unsigned int town::Town::getLinks(node::Node node) {
  unsigned int number_links(0);
  for (size_t i(0); i < links.size(); ++i) {
    if ((links[i].uid1 == node.getUid()) || (links[i].uid2 == node.getUid())) {
      number_links++;
    }
  }
  return number_links;
}

/* === INTERNAL FUNCTIONS === */

namespace {
/** Parses a town from a multiline input stream */
town::Town parseTown(std::istream &stream) {
  std::vector<node::Node> nodes;
  std::vector<node::Link> links;
  std::string line;
  SelectedType selectedType(T_NULL);  // no type set yet

  // Read each line and parse it
  while (readLine(stream, line)) {
    parseLine(line, nodes, links, selectedType);
  }

  town::Town createdTown(nodes, links);
  return createdTown;
}

/**
 * Read a single line from a stream up to a comment delimiter
 * @returns `true` if a line was read, `false` otherwise
 */
bool readLine(std::istream &stream, std::string &line) {
  if (stream.eof()) return false;

  std::string rawLine;
  std::getline(stream, rawLine);
  size_t commentPos(rawLine.find(COMMENT_DELIMITER));
  if (commentPos == std::string::npos) {  // index not found
    line = rawLine;
  } else {
    line = rawLine.substr(0, commentPos);
  }
  return true;
}

/** Parse a line of the town format */
void parseLine(std::string &line, std::vector<node::Node> &nodes,
               std::vector<node::Link> &links, SelectedType &type) {
  std::stringstream stream(line);
  std::vector<std::string> words;

  readWords(stream, words);  // split string into words
  if (words.size() == 0) return;

  // Change the type selection
  if (startsWith(words[0], "nbNodeL")) {
    type = T_HOUSING;
  } else if (startsWith(words[0], "nbNodeT")) {
    type = T_TRANSPORT;
  } else if (startsWith(words[0], "nbNodeP")) {
    type = T_PRODUCTION;
  } else if (startsWith(words[0], "nbLink")) {
    type = T_LINK;
  } else {
    // create the selected type
    if (type == T_LINK) {
      parseLink(links, words);
    } else {
      parseNode(nodes, words, type);
    }
  }
}

/** Splits all space-separated words and replace vector<string>'s contents */
void readWords(std::istream &stream, std::vector<std::string> &words) {
  words.clear();
  std::string word;

  while (stream >> word) {
    words.push_back(word);
  }
}

/** Parses node creation parameters that are required to create a node */
void parseNode(std::vector<node::Node> &nodes, std::vector<std::string> &words,
               SelectedType &selectedType) {
  int uid(stoi(extractValue(words[0])));
  double x(stoi(extractValue(words[1])));
  double y(stoi(extractValue(words[2])));
  int pop(stoi(extractValue(words[3])));

  node::NodeType type =
      selectedType == T_HOUSING
          ? node::HOUSING
          : selectedType == T_TRANSPORT ? node::TRANSPORT : node::PRODUCTION;

  nodes.push_back(node::Node(type, uid, {x, y}, pop));
}

/** Parse a link creation line and add to the links vector */
void parseLink(std::vector<node::Link> &links,
               std::vector<std::string> &words) {
  int uid1(stoi(extractValue(words[0])));
  int uid2(stoi(extractValue(words[1])));

  links.push_back({uid1, uid2});
}

std::string extractValue(const std::string word) {
  int index(getFirstNumberIndex(word));
  return word.substr(index);
}

/** Get the index of the first numerical charcater in a string */
int getFirstNumberIndex(const std::string word) {
  size_t END(word.length());
  for (size_t i(0); i < END; ++i) {
    if (isdigit(word[i])) return i;
  }
  return -1;
}

/** Check if a sample string starts with a character array */
bool startsWith(const std::string sample, const std::string term) {
  if (sample.length() < term.length()) return false;
  return sample.substr(0, term.length()) == term;
}

}  // namespace
