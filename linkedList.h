#pragma once
#include <iostream>
using namespace std;

template <typename T>
struct list_node {
  T data;
  list_node<T>* next;
};

template <typename T>
class linkedList {
 public:
  list_node<T>* head;
  list_node<T>* tail;
  int count;
  linkedList();
  void insert_front(T data);
  void insert_back(T data);
  bool remove(T target);
  void remove_node(list_node<T>* prev, list_node<T>* target);
  void clear();
  bool is_empty() const;
  int get_size() const;
};

template <typename T>
linkedList<T>::linkedList() {
  head = nullptr;
  tail = nullptr;
  count = 0;
}

template <typename T>
bool linkedList<T>::is_empty() const {
  return head == nullptr;
}

template <typename T>
int linkedList<T>::get_size() const {
  return count;
}

template <typename T>
void linkedList<T>::insert_front(T data) {
  list_node<T>* new_node = new list_node<T>;
  new_node->data = data;
  new_node->next = head;
  head = new_node;
  if (tail == nullptr) {
    tail = new_node;
  }
  count++;
}

template <typename T>
void linkedList<T>::insert_back(T data) {
  list_node<T>* new_node = new list_node<T>;
  new_node->data = data;
  new_node->next = nullptr;
  if (is_empty()) {
    head = new_node;
    tail = new_node;
  }
  else {
    tail->next = new_node;
    tail = new_node;
  }
  count++;
}

template <typename T>
void linkedList<T>::remove_node(list_node<T>* prev, list_node<T>* target) {
  if (target == nullptr) {
    return;
  }
  if (target == head) {
    head = target->next;
    if (head == nullptr) {
      tail = nullptr;
    }
  }
  else {
    if (prev == nullptr) {
      return;
    }
    prev->next = target->next;
    if (target == tail) {
      tail = prev;
    }
  }
  delete target;
  count--;
}

template <typename T>
bool linkedList<T>::remove(T target) {
  if (is_empty()) {
    return false;
  }
  list_node<T>* prev = nullptr;
  list_node<T>* curr = head;
  while (curr != nullptr) {
    if (curr->data == target) {
      remove_node(prev, curr);
      return true;
    }
    prev = curr;
    curr = curr->next;
  }
  return false;
}

template <typename T>
void linkedList<T>::clear() {
  while (!is_empty()) {
    list_node<T>* temp = head;
    head = head->next;
    delete temp;
  }
  tail = nullptr;
  count = 0;
}
