// archipelago - model/error.hpp
// Functions for standard execution errors

#include "error.hpp"
#include "constants.hpp"

using namespace std;

/* === Internal function prototypes === */
namespace {
void sort_uid(unsigned int& uid1, unsigned int& uid2);
}

/* === External functions === */

string error::identical_uid(unsigned int uid) {
  return string("Impossible to have two identical uids: ") + to_string(uid) +
         string("\n");
}

string error::link_vacuum(unsigned int uid) {
  return string("No node correspond for this link. Node not found: ") +
         to_string(uid) + string("\n");
}

string error::max_link(unsigned int uid) {
  return string("Too many connections for node: ") + to_string(uid) +
         string("\n");
}

string error::multiple_same_link(unsigned int uid1, unsigned int uid2) {
  sort_uid(uid1, uid2);
  return string("Link already exists inbetween nodes ") + to_string(uid1) +
         string(" and ") + to_string(uid2) + string("\n");
}

string error::node_link_superposition(unsigned int uid) {
  return string(
             "Impossible to have superposition between a link and a node. Node "
             "is ") +
         to_string(uid) + string("\n");
}

string error::node_node_superposition(unsigned int uid1, unsigned int uid2) {
  sort_uid(uid1, uid2);
  return string("Impossible to have superposition between two nodes: ") +
         to_string(uid1) + string(" with ") + to_string(uid2) + string("\n");
}

string error::reserved_uid() {
  return string("Impossible to have the reserved uid\n");
}

string error::success() { return string("Correct file\n"); }

string error::self_link_node(unsigned int uid) {
  return string("Impossible to self-link a node: ") + to_string(uid) +
         string(" <-> ") + to_string(uid) + string("\n");
}

string error::too_little_capacity(unsigned int capacity) {
  return string("Impossible to have a too little capacity: ") +
         to_string(capacity) + string(" < ") + to_string(MIN_CAPACITY) +
         string("\n");
}

string error::too_much_capacity(unsigned int capacity) {
  return string("Impossible to have a too much capacity: ") +
         to_string(capacity) + string(" > ") + to_string(MAX_CAPACITY) +
         string("\n");
}

/* === Internal functions === */
namespace {
void sort_uid(unsigned int& uid1, unsigned int& uid2) {
  if (uid1 > uid2) {
    unsigned int uid_tmp(uid2);
    uid2 = uid1;
    uid1 = uid_tmp;
  }
}
}  // namespace
