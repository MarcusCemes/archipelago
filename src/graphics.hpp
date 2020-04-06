// archipelago - graphics.hpp
// Handles canvas-based rendering

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <gtkmm/drawingarea.h>
#include <gtkmm/window.h>

#include "model/tools.hpp"
#include "model/town.hpp"

namespace graphics {

/**
 * An adapter that extends an abstract RenderContext and provides
 * methods to draw to a Cairo context. An instance of CairoContext
 * is passed to the model.
 */
class CairoContext : public tools::RenderContext {
 public:
  void setContext(const Cairo::RefPtr<Cairo::Context>& context);

  /* Inherited methods */
  void draw(const tools::Circle& obj) override;
  void draw(const tools::Line& obj) override;
  void setColour(const tools::Colour& colour) override;

 private:
  /** A reference to the Cairo context to draw to */
  Cairo::RefPtr<Cairo::Context> cr;
};

/**
 * A GTKmm Widget that draws a town inside of a drawing area.
 *
 * Creates a Cairo context and provides an abstract interface that a town can
 * draw itself into. Updating the town pointer allows a new town to be drawn.
 */
class TownView : public Gtk::DrawingArea {
 public:
  TownView(const town::Town& town);
  void setTown(const town::Town& town);

 protected:
  bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

 private:
  const town::Town* town = nullptr;
  CairoContext context;
};

}  // namespace graphics

#endif
