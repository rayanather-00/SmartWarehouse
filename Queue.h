#pragma once
#include <iostream>

using namespace std;

template <typename T>
struct queue_node {
  T data;
  queue_node<T>* next;
};

template <typename T>
class queue {
 public:
  queue_node<T>* front;
  queue_node<T>* rear;
  int count;
  queue();
  void enqueue(T data);
  void push_front(T data);
  void dequeue(T& removed);
  bool remove(T target);
  bool is_empty();
  int get_size();
  void peek(T& front_data);
};

template <typename T>
queue<T>::queue() {
  front = NULL;
  rear = NULL;
  count = 0;
}

template <typename T>
bool queue<T>::is_empty() {
  return front == NULL;
}

template <typename T>
int queue<T>::get_size() {
  return count;
}

template <typename T>
void queue<T>::enqueue(T data) {
  queue_node<T>* new_node = new queue_node<T>;
  new_node->data = data;
  new_node->next = NULL;
  if (is_empty()) {
    front = new_node;
    rear = new_node;
  }
  else {
    rear->next = new_node;
    rear = new_node;
  }
  count++;
}

template <typename T>
void queue<T>::push_front(T data) {
  queue_node<T>* new_node = new queue_node<T>;
  new_node->data = data;
  new_node->next = front;
  front = new_node;
  if (rear == NULL) {
    rear = new_node;
  }
  count++;
}

template <typename T>
void queue<T>::dequeue(T& removed) {
  if (is_empty()) {
    cout << "queue is empty\n";
    return;
  }
  queue_node<T>* temp = front;
  removed = temp->data;
  front = front->next;
  if (front == NULL) {
    rear = NULL;
  }
  delete temp;
  count--;
}

// remove a given target
template <typename T>
bool queue<T>::remove(T target) {
  if (is_empty()) {
    return false;
  }
  if (front->data == target) {
    queue_node<T>* temp = front;
    front = front->next;
    if (front == NULL) {
      rear = NULL;
    }
    delete temp;
    count--;
    return true;
  }
  queue_node<T>* prev = front;
  queue_node<T>* curr = front->next;
  while (curr != NULL) {
    if (curr->data == target) {
      prev->next = curr->next;
      if (curr == rear) {
        rear = prev;
      }
      delete curr;
      count--;
      return true;
    }
    prev = curr;
    curr = curr->next;
  }
  return false;
}

template <typename T>
void queue<T>::peek(T& front_data) {
  if (is_empty()) {
    cout << "queue is empty\n";
  }
  else {
    front_data = front->data;
  }
}
