#pragma once

class queue_data {
public:
    int order_id; 
    int customeer_order;
    int items;
    int status;
};

struct queue_node {
    queue_data data;
    queue_node* next;
};

class queue{
public:
    queue_node* front;
    queue_node* rear;
    queue();
    void enqueue(queue_data data);
    void dequeue(queue_data& removed);
    bool is_empty();
    void peek();
    void display();
};
