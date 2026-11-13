#include "Queue.h"
#include <iostream>
using namespace std;

queue::queue() {
    front = NULL;
    rear = NULL;
}

bool queue::is_empty() {
    return front == NULL;
}

void queue::enqueue(queue_data data) {
    queue_node* new_node = new queue_node;
    new_node->data = data;
    new_node->next = nullptr;
    if (is_empty()) {
        front = new_node;
        rear = new_node;
    }
    else {
        rear->next = new_node;
        rear = new_node;
    }
}

void queue::dequeue(queue_data& removed) {
    if (is_empty()) {
        cout << "queue is empty\n";
    }
    queue_node* temp = front;
    removed = temp->data;
    front = front->next;
    if (front == NULL) {
        rear = NULL;
    }
    delete temp;
}

void queue::peek() {
    if (is_empty()) {
        cout << "queue is empty\n";
        return;
    }
    cout << "order ID: " << front->data.order_id << endl;
    cout << "customer ID: " << front->data.customeer_order << endl;
    cout << "items: " << front->data.items << endl;
    cout << "ststus: " << front->data.status << endl;
}

void queue::display() {
    if (is_empty())  {
        cout << "queue is empty." << endl;
        return;
    }
    queue_node* temp = front;
    while (temp != NULL) {
        cout << "order ID: " << temp->data.order_id << endl;
        cout << "customer ID: " << temp->data.customeer_order << endl;
        cout << "items: " << temp->data.items << endl;
        cout << "ststus: " << temp->data.status << endl;
        temp = temp->next;
    }
}
