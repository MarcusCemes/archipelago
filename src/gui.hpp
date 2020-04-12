// archipelago - gui.hpp
// Graphical User Interface of the program

#ifndef GUI_H
#define GUI_H

#include <memory>
#include <string>

namespace gui {

int init(const std::unique_ptr<std::string>& path);

}  // namespace gui

#endif
