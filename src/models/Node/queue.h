#ifndef QUEUE_H
#define QUEUE_H
#include <mcpp/mcpp.h>
#include "Node.h"

class Queue {
    private:
    Node* head;

    public:
    Queue();
    void push(mcpp::Coordinate);
    mcpp::Coordinate getCurrent();
    void popFront();
    bool hasNext();
    void push_front(mcpp::Coordinate);
    bool isEmpty();
};
#endif
