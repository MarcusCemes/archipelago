// archipelago v2.0.0 - architecture b2
// project.cpp - program entry point
// Authors: Marcus Cemes, Alexandre Dodens

#include <memory>
#include <string>

#include "gui.hpp"
#include "model/town.hpp"

constexpr unsigned BASIC_ARGS(1);
constexpr unsigned PATH_ARG(1);

/** Parse CLI args and run the program */
int main(int argc, char *argv[]) {
  std::unique_ptr<std::string> path;
  if (argc > BASIC_ARGS) path.reset(new std::string(argv[PATH_ARG]));

  return gui::init(path);
}
