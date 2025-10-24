#ifndef NODE_H
#define NODE_H
#include <mcpp/mcpp.h>

struct Node {
    Node* next;
    mcpp::Coordinate data;
    Node(mcpp::Coordinate coord) : next(nullptr), data(coord) {}
};
#endif
