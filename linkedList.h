#pragma once
#include <iostream>

using namespace std;

template <typename T>
struct list_node {
  T data;
  list_node<T>* next;
};

template <typename T>
class linked_list {
 public:
  list_node<T>* head;
  list_node<T>* tail;
  int count;
  linked_list();
  void insert_front(T data);
  void insert_back(T data);
  bool remove(T target);
  void remove_node(list_node<T>* prev, list_node<T>* target);
  void clear();
  bool is_empty();
  int get_size();
};

template <typename T>
linked_list<T>::linked_list() {
  head = NULL;
  tail = NULL;
  count = 0;
}

template <typename T>
bool linked_list<T>::is_empty() {
  return head == NULL;
}

template <typename T>
int linked_list<T>::get_size() {
  return count;
}

template <typename T>
void linked_list<T>::insert_front(T data) {
  list_node<T>* new_node = new list_node<T>;
  new_node->data = data;
  new_node->next = head;
  head = new_node;
  if (tail == NULL) {
    tail = new_node;
  }
  count++;
}

template <typename T>
void linked_list<T>::insert_back(T data) {
  list_node<T>* new_node = new list_node<T>;
  new_node->data = data;
  new_node->next = NULL;
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
void linked_list<T>::remove_node(list_node<T>* prev, list_node<T>* target) {
  if (target == NULL) {
    return;
  }
  if (target == head) {
    head = target->next;
    if (head == NULL) {
      tail = NULL;
    }
  }
  else {
    if (prev == NULL) {
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
bool linked_list<T>::remove(T target) {
  if (is_empty()) {
    return false;
  }
  list_node<T>* prev = NULL;
  list_node<T>* curr = head;
  while (curr != NULL) {
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
void linked_list<T>::clear() {
  while (!is_empty()) {
    list_node<T>* temp = head;
    head = head->next;
    delete temp;
  }
  tail = NULL;
  count = 0;
}
