// archipelago - gui.cpp
// Graphical User Interface of the program

#include "gui.hpp"

#include <gtkmm/grid.h>
#include <gtkmm/window.h>

#include "graphics.hpp"
#include "model/constants.hpp"

namespace {

/* === CONSTANTS, DECLARATIONS & PROTOTYPES === */

class Content : public Gtk::Grid {
 public:
  Content(const town::Town& town);

 private:
  graphics::TownView townView;
};

class Window : public Gtk::Window {
 public:
  Window(const town::Town& town);

 private:
  Content content;
};

int start(town::Town& town);

}  // namespace

namespace gui {

/* === FUNCTIONS === */

int init() {
  town::Town town;
  return start(town);
}

int init(char* path) {
  town::Town town(town::start(path));
  return start(town);
}

}  // namespace gui

namespace {

int start(town::Town& town) {
  auto app(Gtk::Application::create("ch.epfl.archipelago-301366_301070"));

  Window window(town);
  window.show_all();

  return app->run(window);
}

/* === LAYOUT === */

Content::Content(const town::Town& town) : townView(town) {
  // Not magic numbers, coordinates where to attach each layout element
  townView.set_hexpand(true);
  townView.set_vexpand(true);
  attach(townView, 1, 0, 1, 1);
}

Window::Window(const town::Town& town) : content(town) {
  set_default_size(DEFAULT_DRAWING_SIZE, DEFAULT_DRAWING_SIZE);
  set_title("Archipelago Town Editor");

  add(content);

  show_all();
}

}  // namespace
