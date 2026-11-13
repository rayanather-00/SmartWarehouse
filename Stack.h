#pragma once 

class operation_record { // chnage it later
public:
    string operation_type;  // like what operation we doing with it 
    int record_id;  // this storing the id of the product or whatever we use this for
    int previous_quantity;  
    double previous_price;  
    int previous_status;  
};

class stack_node {
public:
    operation_record data;  
    stack_node* next;  
};

class stack {
public:
    stack_node* top; 
    stack();  
    void push(operation_record data);  
    void pop(operation_record& removed);  
    void peek(operation_record& top_record);  
    bool is_empty();  
    void display(); 
};
