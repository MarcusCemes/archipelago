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

void collison_quartiers (Node w, Node u) {
    Vecteur dist;
    double dist_norme;
    dist.x= w.getPos().x-u.getPos().x
    dist.y= w.getPos().y-u.getPos().y
    dist_norme = sqrt(dist.x*dist.x+dist.y*dist.y);
    if (dist_norme <= w.getRadius()+u.getRadius())
        node_node_superposition(unsigned int uid1, unsigned int uid2);
}

void collision_lien_quartier (Node w, Node u, Node v) {
    if (dist_min_point_droite (w.getPos(), u.getPos(), v.getPos()) > w.getRadius()) 
        node_link_superposition(w.getRadius());
}

void connexions_logements(Node w) {
    if (getLinks(w) > MAX_LINK) 
        max_link(w.getCapacity()); 
}

void min_capacite (Node node) {
    if (node.getCapacity()<MIN_CAPACITY) 
        too_little_capacity(unsigned int node.getCapacity());
}

void max_capacite (Node node) {
    if (node.getCapacity()>MAX_CAPACITY) 
        too_much_capacity(unsigned int node.getCapacity());
}




