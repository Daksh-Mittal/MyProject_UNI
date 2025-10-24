#include "queue.h"
#include <mcpp/mcpp.h>
#include "Node.h"

Queue::Queue() {
    this->head = nullptr;
}
//Queue system whtat adds a new coordinate to the end of the queue
void Queue::push(mcpp::Coordinate coord) {
     Node* newNode = new Node(coord); //create a new node that stores the new coordinates;

     //checks if the head pointer is empty
    if (!head) { 
        head = newNode;
        return;
    }

    //if not, create a new node and make it equal to the head
    Node* curr = head;
    while (curr->next) curr = curr->next; //cycle through the linked lsit until it reaches the last term
    curr->next = newNode; //make the last term point to the new coordinate
}

//checks if queue is empty
bool Queue::isEmpty() {
    return head == nullptr;
}

//add node to front
void Queue::push_front(mcpp::Coordinate coord) {
    Node* newNode = new Node(coord);
    newNode->next = head;
    head = newNode;
}


mcpp::Coordinate Queue::getCurrent() {
    return head->data;
}


bool Queue::hasNext() {
    bool hasNext = false;
    if (!head->next) {
        hasNext = false;
    } else {
        hasNext = true;
    }
    return hasNext;
}

//destroy the node at front
void Queue::popFront() {
    if (!head) {
        return;
    }

    Node* temp = head;
    head = head->next;
    delete temp;
    return;
}
