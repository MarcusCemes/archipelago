// archipelago v2.0.0 - architecture b2
// graphics.cpp - canvas rendering
// Authors: Marcus Cemes, Alexandre Dodens

#include "graphics.hpp"

#include <gtkmm/drawingarea.h>

#include "model/constants.hpp"
#include "model/tools.hpp"
#include "model/town.hpp"

namespace {

constexpr double CENTRE(.5);

constexpr double PI(3.141592653589793238);
constexpr double ZERO(0.);
constexpr double D_PI(PI * 2);
constexpr int TWO(2);

constexpr double WHITE[3]{1., 1., 1.};
constexpr double BLACK[3]{0., 0., 0.};
constexpr double ORANGE[3]{1., 133. / 256., 27. / 256.};
constexpr double GREEN[3]{0., 1., 0.};

constexpr double STROKE_WIDTH(6.);

double calculateScale(double width, double height, double zoom);

}  // namespace

namespace graphics {

/*== Renderer == */

TownView::TownView(const std::shared_ptr<town::Town>& town, double initialZoom)
    : town(town), zoomFactor(initialZoom) {}

bool TownView::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
  Gtk::Allocation allocation = get_allocation();
  const double width(allocation.get_width());
  const double height(allocation.get_height());
  const double scale(calculateScale(width, height, zoomFactor));

  // World to screen space coordinate transformation
  // World objects are symmetrical, so flipping has no effect on visuals
  cr->translate(width / TWO, height / TWO);
  cr->scale(scale, -scale);

  // Erase and paint the background
  cr->save();
  cr->set_source_rgb(WHITE[0], WHITE[1], WHITE[2]);
  cr->paint();
  cr->restore();

  context.setContext(cr);
  if (town) town->render(context);

  return true;
}

void TownView::setZoom(double newZoom) {
  zoomFactor = newZoom;
  queue_draw();
}

/* == Cairo context == */

CairoContext::CairoContext() : colour(tools::Colour::BLACK) {}

void CairoContext::setContext(const Cairo::RefPtr<Cairo::Context>& newContext) {
  cr = newContext;
}

void CairoContext::draw(const tools::Circle& obj) {
  tools::Vec2 position(obj.getPosition());

  cr->save();
  cr->arc(position.getX(), position.getY(), obj.getRadius(), ZERO, D_PI);
  cr->set_source_rgb(WHITE[0], WHITE[1], WHITE[2]);
  cr->fill_preserve();
  cr->set_line_width(STROKE_WIDTH);
  setSourceFromColour();
  cr->stroke();
  cr->restore();
}

void CairoContext::draw(const tools::Line& obj) {
  cr->save();
  cr->move_to(obj.getPointA().getX(), obj.getPointA().getY());
  cr->line_to(obj.getPointB().getX(), obj.getPointB().getY());
  cr->set_line_width(STROKE_WIDTH);
  setSourceFromColour();
  cr->stroke();
  cr->restore();
}

void CairoContext::draw(const tools::Polygon4& obj) {
  cr->save();
  cr->move_to(obj.getA().getX(), obj.getA().getY());
  cr->line_to(obj.getB().getX(), obj.getB().getY());
  cr->line_to(obj.getC().getX(), obj.getC().getY());
  cr->line_to(obj.getD().getX(), obj.getD().getY());
  cr->close_path();
  cr->set_line_width(STROKE_WIDTH);
  setSourceFromColour();
  cr->stroke();
  cr->restore();
}

void CairoContext::setColour(const tools::Colour& newColour) { colour = newColour; }

void CairoContext::setSourceFromColour() {
  switch (colour) {
    case tools::Colour::BLACK:
      cr->set_source_rgb(BLACK[0], BLACK[1], BLACK[2]);
      break;
    case tools::Colour::ORANGE:
      cr->set_source_rgb(ORANGE[0], ORANGE[1], ORANGE[2]);
      break;
    case tools::Colour::GREEN:
      cr->set_source_rgb(GREEN[0], GREEN[1], GREEN[2]);
      break;
  }
}

}  // namespace graphics

namespace {

double calculateScale(double width, double height, double zoom) {
  double smallestSide(width <= height ? width : height);
  return zoom * smallestSide / (TWO * DIM_MAX);
}

}  // namespace
