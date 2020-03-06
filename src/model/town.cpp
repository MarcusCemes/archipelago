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
using namespace std;
using namespace node;

/* === INTERNAL DEFINITIONS AND PROTOTYPES === */

namespace {
/** Last selected type to be created */
enum SelectedType { T_NULL, T_HOUSING, T_TRANSPORT, T_PRODUCTION, T_LINK };

constexpr char COMMENT_DELIMITER('#');
constexpr char NULL_CHAR('\0');

void parseTown(istream &stream);
bool readLine(istream &stream, string &line);
void parseLine(string &line, vector<Node> &nodes, vector<Link> &links,
               SelectedType &selectedType);
void readWords(istream &stream, vector<string> &words);
void parseNode(vector<Node> &nodes, vector<string> &words,
               SelectedType &selectedType);
void parseLink(vector<Link> &links, vector<string> &words);
string extractKey(string &word);
string extractValue(string &word);
bool startsWith(string sample, string term);
int getFirstNumberIndex(string &word);
}  // namespace


/* === FUNCTIONS === */

namespace town {

/** Load and parse a town from a file */
void loadFromFile(char *path) {
  ifstream file(path);
  if (file.is_open()) {
    parseTown(file);
  } else {
    cerr << "File not open" << endl;
  }
}

}

/* === INTERNAL FUNCTIONS === */

namespace {
/** Parses a town from a multiline input stream */
void parseTown(istream &stream) {
  vector<Node> nodes;
  vector<Link> links;
  string line;
  SelectedType selectedType(T_NULL);  // no type set yet

  // Read each line and parse it
  while (readLine(stream, line)) {
    parseLine(line, nodes, links, selectedType);
  }
}

/**
 * Read a single line from a stream up to a comment delimiter
 * @returns `true` if a line was read, `false` otherwise
 */
bool readLine(istream &stream, string &line) {
  if (stream.eof()) return false;

  string rawLine;
  getline(stream, rawLine);
  size_t commentPos(rawLine.find(COMMENT_DELIMITER));
  if (commentPos == string::npos) {
    line = rawLine;
  } else {
    line = rawLine.substr(0, commentPos);
  }
  return true;
}

/** Parse a line of the town format */
void parseLine(string &line, vector<Node> &nodes, vector<Link> &links,
               SelectedType &type) {
  stringstream stream(line);
  vector<string> words;

  readWords(stream, words);  // split string into words
  if (words.size() == 0) return;

  // Change the type selection
  if (startsWith(words[0], "nbNodeH")) {
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
void readWords(istream &stream, vector<string> &words) {
  words.clear();
  string word;

  while (stream >> word) {
    words.push_back(word);
  }
}

void parseNode(vector<Node> &nodes, vector<string> &words,
               SelectedType &selectedType) {
  int uid(stoi(extractValue(words[0])));
  double x(stoi(extractValue(words[1])));
  double y(stoi(extractValue(words[2])));
  int pop(stoi(extractValue(words[3])));

  NodeType type = selectedType == T_HOUSING
                      ? HOUSING
                      : selectedType == T_TRANSPORT ? TRANSPORT : PRODUCTION;

  nodes.push_back(Node(type, uid, {x, y}, pop));
}

/** Parse a link creation line and add to the links vector */
void parseLink(vector<Link> &links, vector<string> &words) {
  int uid1(stoi(extractValue(words[0])));
  int uid2(stoi(extractValue(words[1])));

  links.push_back({uid1, uid2});
}

string extractKey(string &word) {
  int index(getFirstNumberIndex(word));
  return word.substr(0, index);
}

string extractValue(string &word) {
  int index(getFirstNumberIndex(word));
  return word.substr(index);
}

/** Get the index of the first numerical charcater in a string */
int getFirstNumberIndex(string &word) {
  size_t END(word.length());
  for (size_t i(0); i < END; ++i) {
    if (isdigit(word[i])) return i;
  }
  return -1;
}

/** Check if a sample string starts with a character array */
bool startsWith(string sample, string term) {
  return sample > term;
}

}  // namespace
