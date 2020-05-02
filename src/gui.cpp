// archipelago - gui.cpp
// Graphical User Interface of the program

#include "gui.hpp"

#include <gtkmm/application.h>
#include <gtkmm/box.h>  // control housing
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>          // control housing
#include <gtkmm/filechooserdialog.h>  // file picker
#include <gtkmm/frame.h>              // control housing
#include <gtkmm/grid.h>               // main content layout
#include <gtkmm/label.h>              // zoom level
#include <gtkmm/messagedialog.h>
#include <gtkmm/radiobutton.h>  // node selectors
#include <gtkmm/window.h>
#include <sigc++/connection.h>        // data store
#include <sigc++/functors/mem_fun.h>  // data store
#include <sigc++/signal.h>            // data store

#include <memory>   // shared_ptr
#include <sstream>  // ostringstream
#include <string>

#include "graphics.hpp"
#include "model/constants.hpp"

namespace {

/* === CONSTANTS, DECLARATIONS & PROTOTYPES === */

constexpr char APP_ID[]("ch.epfl.archipelago-301366_301070");
constexpr char WINDOW_TITLE[]("Archipelago Town Editor");

constexpr int SPACING(4);
constexpr double INITIAL_ZOOM(1.0);

constexpr int ENJ_PRECISION(4);
constexpr int CI_PRECISION(5);
constexpr int MTA_PRECISION(2);
constexpr int MTA_FIXED_PRECISION(0);
constexpr int ZOOM_PRECISION(1);
constexpr double MTA_FIXED_LIMIT(1E4);

/** DELTA_ZOOM is not perfectly representable as a binary floating point number */
constexpr double ZOOM_ERROR(1E-10);

/** Actions that can be triggered by the interface and dispatched to the store */
enum Action {
  EXIT,
  NEW,
  OPEN,
  SAVE,
  SHORTEST_PATH,
  ZOOM_IN,
  ZOOM_OUT,
  ZOOM_RESET,
  EDIT_LINK
};

enum SelectedNode { HOUSING, TRANSPORT, PRODUCTION };

typedef sigc::signal<void> UpdateSignal;
typedef sigc::signal<void, Action> ActionSignal;

/**
 * The application data store. Contains pointers to data structures as well as
 * simple interface state. Two event streams (signals) are exposed that allow
 * widgets to subscribe to data changes or dispatch events.
 *
 * The update change signifies that the GUI should be updated to match the
 * store, and the action stream publishes actions that should be processed by
 * the controller.
 */
class Store {
 public:
  Store();

  UpdateSignal getUpdateSignal();
  ActionSignal getActionSignal();

  std::shared_ptr<town::Town> getTown();

  double getZoomFactor() const;
  SelectedNode getSelectedNode() const;

  void setZoomFactor(double newValue);
  void setSelectedNode(const SelectedNode& node);

 private:
  UpdateSignal updateSignal;
  ActionSignal actionSignal;

  std::shared_ptr<town::Town> town;

  SelectedNode selectedNode;
  double zoomFactor;
};

/** Shorthand to a C++11 shared pointer of a store instance */
typedef std::shared_ptr<Store> SharedStore;

/**
 * Abstract class that subscribes to a stores update stream, and calls the
 * onUpdate() method with a shared pointer to the store.
 */
class Subscription {
 public:
  Subscription() = delete;
  Subscription(SharedStore& store);
  ~Subscription();

 protected:
  virtual void onUpdate(SharedStore& store) = 0;

 private:
  SharedStore store;
  sigc::connection connection;

  void triggerUpdate();
};

/** The brain of the GUI, handles actions and updates the central store. */
class Controller {
 public:
  Controller() = delete;
  Controller(Gtk::Window& window);
  ~Controller();

  SharedStore& getStore();

  void loadTown(const std::string& path);
  void handleAction(const Action& action);

 private:
  Gtk::Window* window;  // interally used for dialogues
  SharedStore store;
  sigc::connection connection;

  void openTown();
  void saveTown();

  void noAction();
  void changeZoom(double newZoom, bool absolute = false);
};

/** Extended Gtk::Button that dispatches a saved action to the store */
class Button : public Gtk::Button {
 public:
  Button() = delete;
  Button(const std::string& text, SharedStore& store, const Action& action);
  ~Button();

 private:
  /** The configured button action to dispatch */
  Action action;
  SharedStore store;
  sigc::connection connection;
  void handleClick();
};

/** Extended Gtk::Frame with an internal Gtk::Box and better padding */
class Group : public Gtk::Frame {
 public:
  Group() = delete;
  Group(const std::string& label);

  void add(Widget& widget);

 private:
  Gtk::Box buttonBox;
};

/** Live data element that displays the zoom level */
class ZoomLabel : public Gtk::Label, public Subscription {
 public:
  ZoomLabel() = delete;
  ZoomLabel(SharedStore& store);

 private:
  void onUpdate(SharedStore& store) override;
};

/** Live data element that displays the enj statistic */
class EnjLabel : public Gtk::Label, public Subscription {
 public:
  EnjLabel() = delete;
  EnjLabel(SharedStore& store);

 private:
  void onUpdate(SharedStore& store) override;
};

/** Live data element that displays the ci statistic */
class CiLabel : public Gtk::Label, public Subscription {
 public:
  CiLabel() = delete;
  CiLabel(SharedStore& store);

 private:
  void onUpdate(SharedStore& store) override;
};

/** Live data element that displays the mta statistic */
class MtaLabel : public Gtk::Label, public Subscription {
 public:
  MtaLabel() = delete;
  MtaLabel(SharedStore& store);

 private:
  void onUpdate(SharedStore& store) override;
};

class Selectors : public Gtk::Box {
 public:
  Selectors() = delete;
  Selectors(SharedStore& store);

 private:
  SharedStore store;
  Gtk::RadioButton housing, production, transport;
  void handleChange();
};

/** Application sidebar that houses the controls */
class Sidebar : public Gtk::Box {
 public:
  Sidebar() = delete;
  Sidebar(SharedStore& store);

 private:
  Group generalGroup, displayGroup, editorGroup, infoGroup;

  Button exitButton, newButton, openButton, saveButton, shortestButton, zoomInButton,
      zoomOutButton, zoomResetButton, editLinkButton;
  Selectors selectors;

  ZoomLabel zoomLabel;
  EnjLabel enjLabel;
  CiLabel ciLabel;
  MtaLabel mtaLabel;
};

/** Extended graphics::TownView that subscribes to the data store */
class Viewport : public Subscription, public graphics::TownView {
 public:
  Viewport() = delete;
  Viewport(SharedStore& store);

  void onUpdate(SharedStore& store) override;
};

/** The main application window */
class Window : public Gtk::Window {
 public:
  Window();

  void loadFile(const std::string& path);

 private:
  Controller controller;  // must be initialised first
  Sidebar sidebar;
  Viewport viewport;
  Gtk::Grid view;
};

}  // namespace

namespace gui {

/* === FUNCTIONS === */

/** Main application entry point that creates a GUI and runs the application */
int init(const std::unique_ptr<std::string>& path) {
  auto app(Gtk::Application::create(APP_ID));
  Window window;

  if (path) window.loadFile(*path);

  return app->run(window);
}

}  // namespace gui

namespace {

/* === DATA === */

/* == Store == */

Store::Store()
    : town(new town::Town()), selectedNode(HOUSING), zoomFactor(INITIAL_ZOOM) {}

ActionSignal Store::getActionSignal() { return actionSignal; }
UpdateSignal Store::getUpdateSignal() { return updateSignal; }

std::shared_ptr<town::Town> Store::getTown() { return town; }
double Store::getZoomFactor() const { return zoomFactor; }
SelectedNode Store::getSelectedNode() const { return selectedNode; }
void Store::setZoomFactor(double newValue) { zoomFactor = newValue; }
void Store::setSelectedNode(const SelectedNode& newValue) { selectedNode = newValue; }

/* == Subscription == */

Subscription::Subscription(SharedStore& store)
    : store(store),
      connection(store->getUpdateSignal().connect(
          sigc::mem_fun(*this, &Subscription::triggerUpdate))) {}
Subscription::~Subscription() { connection.disconnect(); }

void Subscription::triggerUpdate() { onUpdate(store); }

/* == Controller == */

Controller::Controller(Gtk::Window& window)
    : window(&window),
      store(new Store()),
      connection(store->getActionSignal().connect(
          sigc::mem_fun(*this, &Controller::handleAction))) {}
Controller::~Controller() { connection.disconnect(); }

SharedStore& Controller::getStore() { return store; }

void Controller::handleAction(const Action& action) {
  switch (action) {
    case Action::EXIT:
      window->close();  // let the program terminate gracefully
      break;

    case Action::NEW:
      *store->getTown() = town::Town();
      store->getUpdateSignal().emit();
      break;

    case Action::OPEN:
      openTown();
      break;

    case Action::SAVE:
      saveTown();
      break;

    case Action::ZOOM_IN:
      changeZoom(DELTA_ZOOM);
      break;

    case Action::ZOOM_OUT:
      changeZoom(-DELTA_ZOOM);
      break;

    case Action::ZOOM_RESET:
      changeZoom(INITIAL_ZOOM, true);
      break;

    default:
      noAction();
  }
}  // namespace

void Controller::noAction() {
  Gtk::MessageDialog dialog(*window, "You clicked a button!");
  dialog.set_secondary_text("Unfortunately, it doesn't do anything yet");
  dialog.run();
}

void Controller::changeZoom(double zoomFactor, bool absolute) {
  double currentZoom(store->getZoomFactor());
  double newZoom(absolute ? zoomFactor : currentZoom += zoomFactor);
  if (newZoom + ZOOM_ERROR >= MIN_ZOOM && newZoom - ZOOM_ERROR <= MAX_ZOOM) {
    store->setZoomFactor(newZoom);
    store->getUpdateSignal().emit();
  }
}

void Controller::openTown() {
  Gtk::FileChooserDialog dialog(*window, "Open a town", Gtk::FILE_CHOOSER_ACTION_OPEN);
  dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
  dialog.add_button("Select", Gtk::RESPONSE_OK);
  const auto result = dialog.run();
  dialog.close();  // helps avoid conflict with a subsequent error dialog

  if (result == Gtk::RESPONSE_OK) loadTown(dialog.get_filename());
}

void Controller::saveTown() {
  Gtk::FileChooserDialog dialog(*window, "Save town", Gtk::FILE_CHOOSER_ACTION_SAVE);
  dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
  dialog.add_button("Save", Gtk::RESPONSE_OK);
  const auto result = dialog.run();
  dialog.close();  // helps avoid conflict with a subsequent error dialog

  if (result == Gtk::RESPONSE_OK) {
    town::saveToFile(dialog.get_filename(), *store->getTown());
  }
}

void Controller::loadTown(const std::string& path) {
  try {
    *store->getTown() = town::loadFromFile(path);
    store->getUpdateSignal().emit();
  } catch (std::string err) {
    Gtk::MessageDialog dialog(*window, "Could not open file", false,
                              Gtk::MESSAGE_ERROR);
    dialog.set_secondary_text(err);
    dialog.run();
  }
}

/* === LAYOUT === */

/* == Button == */

Button::Button(const std::string& text, SharedStore& store, const Action& action)
    : Gtk::Button(text),
      action(action),
      store(store),
      connection(
          signal_clicked().connect(sigc::mem_fun(*this, &Button::handleClick))) {
  set_margin_bottom(SPACING);
}
Button::~Button() { connection.disconnect(); }

void Button::handleClick() { store->getActionSignal().emit(action); }

/* == Group == */

Group::Group(const std::string& label)
    : Frame(label), buttonBox(Gtk::ORIENTATION_VERTICAL) {
  set_margin_left(SPACING);
  set_margin_right(SPACING);
  Frame::add(buttonBox);
  buttonBox.set_margin_left(SPACING);
  buttonBox.set_margin_right(SPACING);
}

void Group::add(Widget& widget) { buttonBox.add(widget); }

/* == ZoomLabel == */

ZoomLabel::ZoomLabel(SharedStore& store) : Subscription(store) {}

void ZoomLabel::onUpdate(SharedStore& store) {
  std::ostringstream formatter;
  formatter.setf(std::ios::fixed);
  formatter.precision(ZOOM_PRECISION);
  formatter << store->getZoomFactor();
  set_label("Zoom: x" + formatter.str());
  set_margin_bottom(SPACING);
}

/* == EnjLabel == */

EnjLabel::EnjLabel(SharedStore& store) : Subscription(store) {}

void EnjLabel::onUpdate(SharedStore& store) {
  std::ostringstream formatter;
  formatter << store->getTown()->enj();
  set_label("ENJ: " + formatter.str());
  set_margin_bottom(SPACING);
}

/* == CiLabel == */

CiLabel::CiLabel(SharedStore& store) : Subscription(store) {}

void CiLabel::onUpdate(SharedStore& store) {
  std::ostringstream formatter;
  formatter << store->getTown()->ci();
  set_label("CI: " + formatter.str());
  set_margin_bottom(SPACING);
}

/* == MtaLabel == */

MtaLabel::MtaLabel(SharedStore& store) : Subscription(store) {}

void MtaLabel::onUpdate(SharedStore& store) {
  std::stringstream formatter;
  auto mta(store->getTown()->mta());
  formatter << mta;
  set_label("MTA: " + formatter.str());
  set_margin_bottom(SPACING);
}

/* == Selectors == */

Selectors::Selectors(SharedStore& store)
    : Box(Gtk::ORIENTATION_VERTICAL),
      store(store),
      housing("Housing"),
      production("Production"),
      transport("Transport") {
  production.join_group(housing);
  transport.join_group(housing);
  housing.set_active();

  housing.signal_toggled().connect(sigc::mem_fun(*this, &Selectors::handleChange));
  transport.signal_toggled().connect(sigc::mem_fun(*this, &Selectors::handleChange));
  production.signal_toggled().connect(sigc::mem_fun(*this, &Selectors::handleChange));

  pack_start(housing);
  pack_start(production);
  pack_start(transport);
}

void Selectors::handleChange() {
  if (housing.get_active()) {
    store->setSelectedNode(HOUSING);
  } else if (transport.get_active()) {
    store->setSelectedNode(TRANSPORT);
  } else if (production.get_active()) {
    store->setSelectedNode(PRODUCTION);
  }
}

/* == Sidebar == */

Sidebar::Sidebar(SharedStore& store)
    : Box(Gtk::ORIENTATION_VERTICAL),
      generalGroup("General"),
      displayGroup("Display"),
      editorGroup("Editor"),
      infoGroup("Information"),
      exitButton("Exit", store, Action::EXIT),
      newButton("New", store, Action::NEW),
      openButton("Open", store, Action::OPEN),
      saveButton("Save", store, Action::SAVE),
      shortestButton("Shortest path", store, Action::SHORTEST_PATH),
      zoomInButton("Zoom in", store, Action::ZOOM_IN),
      zoomOutButton("Zoom out", store, Action::ZOOM_OUT),
      zoomResetButton("Zoom reset", store, Action::ZOOM_RESET),
      editLinkButton("Edit link", store, Action::EDIT_LINK),
      selectors(store),
      zoomLabel(store),
      enjLabel(store),
      ciLabel(store),
      mtaLabel(store) {
  generalGroup.add(exitButton);
  generalGroup.add(newButton);
  generalGroup.add(openButton);
  generalGroup.add(saveButton);
  displayGroup.add(shortestButton);
  displayGroup.add(zoomOutButton);
  displayGroup.add(zoomInButton);
  displayGroup.add(zoomResetButton);
  displayGroup.add(zoomLabel);
  editorGroup.add(editLinkButton);
  editorGroup.add(selectors);
  infoGroup.add(enjLabel);
  infoGroup.add(ciLabel);
  infoGroup.add(mtaLabel);

  add(generalGroup);
  add(displayGroup);
  add(editorGroup);
  add(infoGroup);
}

/* == Viewport == */

Viewport::Viewport(SharedStore& store)
    : Subscription(store), TownView(store->getTown(), INITIAL_ZOOM) {}

void Viewport::onUpdate(SharedStore& store) { setZoom(store->getZoomFactor()); }

/* == Window == */

Window::Window()
    : controller(*this),
      sidebar(controller.getStore()),
      viewport(controller.getStore()) {
  set_title(WINDOW_TITLE);

  viewport.set_hexpand(true);
  viewport.set_vexpand(true);
  viewport.set_size_request(DEFAULT_DRAWING_SIZE, DEFAULT_DRAWING_SIZE);

  view.attach(sidebar, 0, 0, 1, 1);
  view.attach(viewport, 1, 0, 1, 1);

  add(view);
  show_all();

  controller.getStore()->getUpdateSignal().emit();
}

void Window::loadFile(const std::string& path) { controller.loadTown(path); }

}  // namespace
