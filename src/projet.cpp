// archipelago - projet.cpp
// Program entry point

#include <memory>
#include <string>

#include "gui.hpp"
#include "model/town.hpp"

/** Parse CLI args and run the program */
int main(int argc, char *argv[]) {
  std::unique_ptr<std::string> path;
  if (argc > 1) path.reset(new std::string(argv[1]));

  return gui::init(path);
}
