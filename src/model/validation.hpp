// archipelago - model/validation.hpp
// Town verification logic

#ifndef MODEL_VALIDATION_H
#define MODEL_VALIDATION_H

#include "node.hpp"
#include "town.hpp"

using namespace node;
using namespace town;

namespace validation {

void duplicateUid(Town town);

void missingNode(Town town);

// void connexions_logements(Node node);

void duplicateLink(Town town);

void linkCollision(Node node, Node node1, Node node2);

void nodeCollision(Node node1, Node node2);

void badUid(Node node);

void selfLink(Town town);

void minCapacity(Node node);

void maxCapacity(Node node);

}  // namespace validation

#endif
