// archipelago v3.0.0 - architecture b2
// graphics.hpp - canvas rendering
// Authors: Marcus Cemes, Alexandre Dodens

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <gtkmm/drawingarea.h>
#include <gtkmm/window.h>
#include <sigc++/connection.h>
#include <sigc++/signal.h>

#include <memory>

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
  CairoContext();
  void setContext(const Cairo::RefPtr<Cairo::Context>& context);

  /* Inherited methods */
  void draw(const tools::Circle& obj) override;
  void draw(const tools::Line& obj) override;
  void draw(const tools::Polygon4& obj) override;
  void setColour(const tools::Colour& colour) override;

 private:
  /** A reference to the Cairo context to draw to */
  Cairo::RefPtr<Cairo::Context> cr;
  tools::Colour colour;

  void setSourceFromColour();
};

/**
 * A GTKmm Widget that draws a town inside of a drawing area.
 *
 * Creates a Cairo context and provides an abstract interface that a town can
 * draw itself into. Updating the town pointer allows a new town to be drawn.
 */
class TownView : public Gtk::DrawingArea {
 public:
  TownView() = delete;
  TownView(const std::shared_ptr<town::Town>& town, double initalZoom);

  void setZoom(double zoom);

 protected:
  bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

 private:
  std::shared_ptr<town::Town> town;
  CairoContext context;
  double zoomFactor;
};

}  // namespace graphics

#endif
