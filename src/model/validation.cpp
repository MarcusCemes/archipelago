// archipelago - model/validation.cpp
// Validate town constraints

#include <cmath>
#include <iostream>
#include <map>
#include <set>

#include "constants.hpp"
#include "error.hpp"
#include "node.hpp"
#include "tools.hpp"
#include "town.hpp"
#include "validation.hpp"

using namespace std;
using namespace error;
using namespace node;
using namespace tools;
using namespace town;

namespace validation {

/**
 * Node & Link validation. Returns the validation status string.
 * Unfortunately, despite issue #26490, the order *does* matter.
 */
string validateAll(const Nodes& nodes, const Links& links) {
  try {
    /** Independent tests - These must be validated first */
    reservedUid(nodes);
    duplicateUids(nodes);
    linkUidsExist(nodes, links);  // should come after `reservedUid`
    duplicateLinks(links);
    validCapacity(nodes);
    badLink(links);

    /* Dependent tests - These depend on the above */
    // e.g. An illegal capacity size may cause a "false" node collision
    // nodeLinkCollision(nodes, links);
    nodeCollision(nodes);
    maxHousingConnections(links);  // This MUST execute after `duplcateLinks`
    return error::success();
  } catch (string& err) {
    return err;
  }
}

/** Iterate over a list of nodes and checkfor duplicate uids */
void duplicateUids(const Nodes& nodes) {
  auto END(nodes.end());
  for (auto i(nodes.begin()); i != END; ++i) {
    for (auto j(nodes.begin()); j != END; ++j) {
      if (j <= i) continue;

      unsigned iUid((*i).getUid()), jUid((*j).getUid());
      if (iUid == jUid) {
        throw identical_uid(iUid);
      }
    }
  }
}

/** Check that each link's nodes exist */
void linkUidsExist(const Nodes& nodes, const Links& links) {
  // Generate an optimised uid set
  vector<unsigned> listOfUids;
  listOfUids.reserve(nodes.size());
  for (const auto& node : nodes) {
    listOfUids.push_back(node.getUid());
  }
  set<unsigned> nodeUids(listOfUids.begin(), listOfUids.end());

  // Iterate over links and check for the existence of the node's uid
  for (const auto& link : links) {
    if (nodeUids.find(link.getUid0()) == nodeUids.end()) {
      throw link_vacuum(link.getUid0());
    }
    if (nodeUids.find(link.getUid1()) == nodeUids.end()) {
      throw link_vacuum(link.getUid1());
    }
  }
}

/** Check for duplicate links */
void duplicateLinks(const Links& links) {
  // Iterate over each pair of links, avoiding duplicate work
  auto END(links.end());
  for (auto i(links.begin()); i != END; ++i) {
    for (auto j(links.begin()); j != END; ++j) {
      if (j <= i) continue;

      // Make use of Link's `operator==` overloading
      if ((*i) == (*j)) {
        throw multiple_same_link((*i).getUid0(), (*i).getUid1());
      }
    }
  }
}

/** Check that all nodes have a allowed capacity */
void validCapacity(const Nodes& nodes) {
  unsigned capacity;
  for (const auto& node : nodes) {
    capacity = node.getCapacity();
    if (capacity < MIN_CAPACITY) throw too_little_capacity(capacity);
    if (capacity > MAX_CAPACITY) throw too_much_capacity(capacity);
  }
}

// TODO - Check for *segment* collisions, not *line* collisions
/** Checks for collision between nodes and links */
void nodeLinkCollision(const Nodes& nodes, const Links& links) {
  // Optimised map of node positions and sizes (pending better node lookup)
  map<unsigned, Vec2> positionMap;
  for (const auto& node : nodes)
    positionMap[node.getUid()] = node.getPosition();

  // Check for collisions
  unsigned uid;
  unsigned radius;
  for (const auto& node : nodes) {
    for (const auto& link : links) {
      uid = node.getUid();
      radius = node.radius();
      if (minPointLineDistance(positionMap[uid], positionMap[link.getUid0()],
                               positionMap[link.getUid1()]) <=
          (radius + DIST_MIN)) {
        throw node_link_superposition(uid);
      }
    }
  }
}

/** Check for collision between nodes with a safety distance */
void nodeCollision(const Nodes& nodes) {
  double distance;
  auto END(nodes.end());
  for (auto i(nodes.begin()); i != END; ++i) {
    for (auto j(nodes.begin()); j != END; ++j) {
      if (j <= i) continue;

      distance = ((*j).getPosition() - (*i).getPosition()).norm();
      if (distance <= (*i).radius() + (*j).radius() + DIST_MIN) {
        throw node_node_superposition((*i).getUid(), (*j).getUid());
      }
    }
  }
}

/** Count connections for each node, and check it falls within regulation */
void maxHousingConnections(const Links& links) {
  // Create an optimised map of (uid, numberConnections) pairs
  map<unsigned, unsigned> connections;
  for (const auto& link : links) {
    // Cool C++ syntax, starts from zero if doesn't exist
    ++connections[link.getUid0()];
    ++connections[link.getUid1()];
  }

  // Count number of connections for each node
  for (const auto& connection : connections) {
    if (connection.second > MAX_LINK) throw max_link(connection.first);
  }
}

/** Check that no nodes are using a reserved uid */
void reservedUid(const Nodes& nodes) {
  for (const auto& node : nodes) {
    if (node.getUid() == NO_LINK) throw reserved_uid();
  }
}

/** Check that there are no links to the same uid */
void badLink(const Links& links) {
  for (const auto& link : links) {
    if (link.getUid0() == link.getUid1()) throw self_link_node(link.getUid1());
  }
}

}  // namespace validation
