// archipelago - graphics.cpp
// Handles canvas-based rendering

#include "graphics.hpp"

#include <gtkmm/drawingarea.h>

#include "model/tools.hpp"
#include "model/town.hpp"

namespace graphics {

constexpr double BG_COLOUR(.95);
constexpr double CENTRE(.5);

constexpr double PI(3.141592653589793238);
constexpr double D_PI(PI * 2);

/*== Renderer == */

TownView::TownView(const town::Town& town) : town(&town) {}

void TownView::setTown(const town::Town& newTown) { town = &newTown; }

bool TownView::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
  Gtk::Allocation allocation = get_allocation();
  const int width = allocation.get_width();
  const int height = allocation.get_height();

  // cr->scale(width, height);
  cr->translate(width / 2, height / 2);

  if (town != nullptr) {
    context.setContext(cr);
    cr->save();
    cr->set_source_rgb(BG_COLOUR, BG_COLOUR, BG_COLOUR);
    cr->paint();

    town->render(context);
    cr->restore();
  }

  return true;
}

/* == Cairo context == */

void CairoContext::setContext(const Cairo::RefPtr<Cairo::Context>& newContext) {
  cr = newContext;
}

/** STUB */
void CairoContext::draw(const tools::Circle& obj) {
  tools::Vec2 position(obj.getPosition());

  cr->save();
  cr->arc(position.getX(), position.getY(), obj.getRadius(), 0.0, D_PI);
  cr->set_source_rgb(0, 0, 0);
  cr->stroke();
  cr->fill();
  cr->restore();
}
void CairoContext::draw(const tools::Line& obj) {
  // STUB
}
void CairoContext::setColour(const tools::Colour& colour) {
  switch (colour) {
    case tools::Colour::BLACK:
      cr->set_source_rgb(0, 0, 0);
      break;
    case tools::Colour::GREEN:
      cr->set_source_rgb(0, 255, 0);
      break;
  }
}

}  // namespace graphics
