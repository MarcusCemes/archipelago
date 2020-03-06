// archipelago - model/town.cpp
// Town-related logic, such as ENJ, CI and MTA calculations

#include <fstream>   // istream
#include <iostream>  // cerr
#include <sstream>   // stringstream
#include <string>
#include <vector>

#include "node.hpp"
#include "town.hpp"

using namespace town;

/* === DEFINITIONS === */

/** The last set node/link creation type */
enum Type { T_NULL, T_HOUSING, T_TRANSPORT, T_PRODUCTION, T_LINK };

// char[] constants for changing the creation type, as contexpr strings aren't
// allowed
constexpr char *K_HOUSING("nbNodeL");
constexpr char *K_TRANSPORT("nbNodTL");
constexpr char *K_PRODUCTION("nbNodeL");
constexpr char *K_LINK("nbLink");

constexpr char COMMENT_DELIMITER('#');

constexpr char* NUMBER_CHAR("0123456789");
constexpr char NULL_CHAR('\0');

/* === INTERNAL PROTOTYPES === */

namespace {
void parseTown(std::istream &stream);
bool readLine(std::istream &stream, std::string &line);
void parseLine(std::string &line, std::vector<node::Node> &nodes,
               Type &selectedType);
void readWords(std::istream &stream, std::vector<std::string> &words);
void createType(std::vector<node::Node> &nodes,
                    std::vector<std::string> &words, Type &selectedType);
bool startsWith(std::string &string, char *item);
}  // namespace

/* === FUNCTIONS === */

/** Load and parse a town from a file */
void loadFromFile(char *path) {
  std::ifstream file(path);
  if (file.is_open()) {
    parseTown(file);
  } else {
    std::cerr << "File not open" << std::endl;
  }
}

/* === INTERNAL FUNCTIONS === */

namespace {
/** Parses a town from a multiline input stream */
void parseTown(std::istream &stream) {
  std::vector<node::Node> nodes;
  std::string line;
  Type selectedType(T_NULL); // no type set yet

  // Read each line and parse it
  while (readLine(stream, line)) {
    parseLine(line, nodes, selectedType);
  }
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
  if (commentPos == std::string::npos) {
    line = rawLine;
  } else {
    line = rawLine.substr(0, commentPos);
  }
  return true;
}

/** Parse a line of the town format */
void parseLine(std::string &line, std::vector<node::Node> &nodes,
               Type &selectedType) {
  std::stringstream stream(line);
  std::vector<std::string> words;

  readWords(stream, words); // split string into words
  if (words.size() == 0) return;

  // Change the type selection
  if (startsWith(words[0], K_HOUSING)) {
    selectedType = T_HOUSING;
  } else if (startsWith(words[0], K_HOUSING)) {
    selectedType = T_TRANSPORT;
  } else if (startsWith(words[0], K_PRODUCTION)) {
    selectedType = T_PRODUCTION;
  } else if (startsWith(words[0], K_LINK)) {
    selectedType = T_LINK;
  } else {
    // create the selected type
    createType(nodes, words, selectedType);
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

void createType(std::vector<node::Node> &nodes,
                    std::vector<std::string> &words, Type &selectedType) {
  std::vector<int> uids;
  int pop;
  double x, y;

  // Decode each keyword
  for (auto const &word : words) {
    if (word >= "uid") {
      uids.push_back(std::stoi(word.substr(3)));
    } else if (word >= "x") {
      x = std::stoi(word.substr(1));
    } else if (word >= "y") {
      y = std::stoi(word.substr(1));
    } else if (word >= "nbp") {
      pop = std::stoi(word.substr(3));
    } else {
      // TODO invalid key
    }
  }

  switch (selectedType) {
    case (T_HOUSING):
      nodes.push_back(node::Housing(uids.at(0), {x, y}, pop));
      break;
    case (T_TRANSPORT):
      nodes.push_back(node::Transport(uids.at(0), {x, y}, pop));
      break;
    case (T_PRODUCTION):
      nodes.push_back(node::Production(uids.at(0), {x, y}, pop));
      break;
    case (T_LINK):
      // TODO create link
      break;
    default:
      // TODO error
  }
}

void splitKeyValue(std::string &string, std::string &key, std::string &value) {

}

/** Check if string starts with character array */
bool startsWith(std::string &string, char *item) {
  size_t stringLength(string.size());
  size_t itemLength(strlen(item));

  if (stringLength < itemLength) return false;

  for (size_t i(0); i < itemLength; ++i) {
    if (item[i] != string[i]) return false;
  }

  return true;
}

/** Returns true if a character is a number, false otherwise */
bool isNumber(char &character) {
  for (size_t i(0); NUMBER_CHAR[i] != NULL_CHAR; ++i) {
    if (character == i[0]) return true;
  }
  return false;
}

}  // namespace
