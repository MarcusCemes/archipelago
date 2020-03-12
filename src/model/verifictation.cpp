#include "constants.hpp"
#include "node.hpp"
#include "error.hpp"
#include "tools.hpp"
#include "town.hpp"

#include <cmath>
#include <iostream>

using namespace error;
using namespace tools;
using namespace node;
using namespace town;

void same_uid (Town town) {
    for (size_t i(0); i < town.getNodes().size()-1; ++i)
        for (size_t j(i+1); j < town.getNodes().size; ++j)
            if (town.getNodes()[i].getUid() == town.getNodes()[j].getUid())
                identical_uid(town.getNodes()[i].getUid());
}

void nonexistant_node (Town town) {
    for (size_t i(0); i < town.getLinks().size(); ++i) {
        bool existant_uid(false);
        for (size_t j(0); j < town.getNodes().size(); ++j) {
            if (town.getLinks()[i].uid0 == town.getNodes()[j].getUid()) {
                existant_uid = true;
            }
        }
        if (!existant_uid) link_vacuum(town.getLinks()[i].uid0);
    }
    for (size_t i(0); i < town.getLinks().size(); ++i) {
        bool existant_uid(false);
        for (size_t j(0); j < town.getNodes().size(); ++j) {
            if (town.getLinks()[i].uid1 == town.getNodes()[j].getUid())
                existant_uid = true;
        }
        if (!existant_uid) link_vacuum(town.getLinks()[i].uid1);
    }
}

void connexions_logements(Node node) {
    if (getLinks(node) > MAX_LINK) 
        max_link(node.getCapacity()); 
}

void same_link (Town town) {
    for (size_t i(0); i < town.getLinks().size()-1; ++i) {
        for (size_t j(i+1); j < town.getLinks().size(); ++j) {
            if (town.getLinks()[i].uid0 == town.getLinks()[j].uid0 || 
            town.getLinks()[i].uid1 == town.getLinks()[j].uid1) {
                multiple_same_link(town.getLinks()[i].uid0, town.getLinks()[i].uid1);
            }
        }
    }
}

void collision_lien_quartier (Node node, Node node1, Node node2) {
    if (dist_min_point_droite (node.getPos(), node1.getPos(), node2.getPos()) 
    > node.getRadius()) 
        node_link_superposition(node.getRadius());
}

void collison_quartiers (Node node1, Node node2) {
    Vecteur dist;
    double dist_norme;
    dist.x= node1.getPos().x-node2.getPos().x;
    dist.y= node1.getPos().y-node2.getPos().y;
    dist_norme = sqrt(dist.x*dist.x+dist.y*dist.y);
    if (dist_norme <= node1.getRadius()+node2.getRadius())
        node_node_superposition(node1.getUid(), node2.getUid());
}

void wrong_uid (Node node) {
    if (node.getUid() == NO_LINK) reserved_uid();
}

void link_same_node (Town town) {
    for (size_t i(0); i < town.getLinks().size(); ++i) 
        if (town.getLinks()[i].uid0 == town.getLinks()[i].uid1)
            self_link_node(town.getLinks()[i].uid0);
}

void min_capacite (Node node) {
    if (node.getCapacity() < MIN_CAPACITY) 
        too_little_capacity(node.getCapacity());
}

void max_capacite (Node node) {
    if (node.getCapacity() > MAX_CAPACITY) 
        too_much_capacity(node.getCapacity());
}














