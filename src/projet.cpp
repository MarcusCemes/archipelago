// archipelago - projet.cpp
// Program entry point

#include <iostream>

#include "model/town.hpp"

using namespace std;

int main(int argc, char *argv[]) {
  if (argc >= 2) {
    town::start(argv[1]);
  } else {
    town::start();
  }

  return 0;
}
