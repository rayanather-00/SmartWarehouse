#include "Stack.h"
#include <iostream>
#include <cstring>
using namespace std;

stack::stack() {
    top = NULL;
}
bool stack::is_empty() {
    return top == NULL;
}

void stack::push(operation_record data) {
    stack_node* new_node = new stack_node;
    new_node->data = data;
    new_node->next = top;
    top = new_node;
}

void stack::pop(operation_record& removed) {
    if (is_empty()) {
        cout << "stack is empty\n";
    }
    stack_node* temp = top;
    removed = temp->data;
    top = top->next;
    delete temp;
}

void stack::peek(operation_record& top_record) {
    if (is_empty()) {
        cout << "stack is empty\n";
    }
    else {
        top_record = top->data;
    }
}

void stack::display() {
    if (is_empty()) {
        cout << "stack is empty\n";
        return;
    }
    stack_node* temp = top;
    while (temp != nullptr)  {
        cout << "operation: " << temp->data.operation_type << endl;
        cout << "record ID: " << temp->data.record_id << endl;
        cout << "previous Quantity: " << temp->data.previous_quantity << endl;
        cout << "previous Price: " << temp->data.previous_price << endl;
        cout << "previous Status: " << temp->data.previous_status << endl;
        cout << "****************\n";
        temp = temp->next;
    }
}
