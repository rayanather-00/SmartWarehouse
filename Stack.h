#pragma once
#include <iostream>

using namespace std;

template <typename T>
class stack_node {
 public:
  T data;
  stack_node<T>* next;
};

template <typename T>
class stack {
 public:
  stack_node<T>* top;
  stack();
  void push(T data);
  void pop(T& removed);
  void peek(T& top_record);
  bool is_empty();
};

template <typename T>
stack<T>::stack() {
  top = NULL;
}

template <typename T>
bool stack<T>::is_empty() {
  return top == NULL;
}

template <typename T>
void stack<T>::push(T data) {
  stack_node<T>* new_node = new stack_node<T>;
  new_node->data = data;
  new_node->next = top;
  top = new_node;
}

template <typename T>
void stack<T>::pop(T& removed) {
  if (is_empty()) {
    cout << "stack is empty\n";
    return;
  }
  stack_node<T>* temp = top;
  removed = temp->data;
  top = top->next;
  delete temp;
}

template <typename T>
void stack<T>::peek(T& top_record) {
  if (is_empty()) {
    cout << "stack is empty\n";
  }
  else {
    top_record = top->data;
  }
}
