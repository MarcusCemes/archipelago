// archipelago - gui.cpp
// Graphical User Interface of the program

#include "gui.hpp"

#include <gtkmm.h>

namespace gui {

int run() {
  auto app = Gtk::Application::create("ch.epfl.archipelago-301366_301070");

  Gtk::Window window;
  window.set_default_size(800, 600);
  window.set_title("Archipelago Town Editor");

  return app->run(window);
}

}  // namespace gui
