// archipelago - projet.cpp
// Program entry point

#include <iostream>

#include "model/town.hpp"

using namespace std;

/* === PROTOTYPES === */

void init();
void init(char *file);
void start(town::Town &town);

/* === PROGRAM === */

int main(int argc, char *argv[]) {
  if (argc >= 2) {
    init(argv[1]);
  } else {
    init();
  }

  return 0;
}

/** Launch the GUI with an empty town */
void init() {
  town::Town town;
  start(town);
}

/** Create a town from saved file and launch the GUI */
void init(char *file) {
  town::Town town(town::loadFromFile(file));
  start(town);
}

/** STUB - Launch the program with the given Town */
void start(town::Town &town) {
  town.getNodes();  // stub, hides -Wunusfed-parameter warning
}