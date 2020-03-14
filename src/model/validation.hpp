// archipelago - model/validation.hpp
// Validate town constraints

#ifndef MODEL_VALIDATION_H
#define MODEL_VALIDATION_H

#include <string>
#include <vector>

#include "node.hpp"
#include "town.hpp"

using namespace std;
using namespace node;
using namespace town;

// Private module definitions
namespace {
typedef std::vector<Node> Nodes;
typedef std::vector<Link> Links;
}  // namespace

namespace validation {

string validateAll(const Nodes& nodes, const Links& Links);

void duplicateUids(const Nodes& nodes);
void linkUidsExist(const Nodes& nodes, const Links& links);
void duplicateLinks(const Links& links);
void nodeLinkCollision(const Nodes& nodes, const Links& links);
void nodeCollision(const Nodes& nodes);
void maxHousingConnections(const Links& links);
void reservedUid(const Nodes& nodes);
void badLink(const Links& links);
void validCapacity(const Nodes& nodes);

}  // namespace validation

#endif
