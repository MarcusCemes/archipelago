// archipelago - projet.cpp
// Program entry point

#include <iostream>

#include "model/town.hpp"

using namespace std;


/* === PROTOTYPES === */

void init(char *file);


/* === PROGRAM === */

int main(int argc, char *argv[]) {
  if (argc != 1) {
    cerr << "No file input specified" << endl;
    cerr << "Usage: " << argv[0] << " path/to/town.txt" << endl;
    return 1;
  }

  town::loadFromFile(argv[1]);
  return 0;
}
