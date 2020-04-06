// archipelago - projet.cpp
// Program entry point

#include <iostream>

#include "gui.hpp"
#include "model/town.hpp"

using namespace std;

int main(int argc, char *argv[]) {
  if (argc >= 2) {
    return gui::init(argv[1]);
  } else {
    return gui::init();
  }
}
