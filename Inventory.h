#pragma once
#include <iostream>
#include <string>
#include "linkedList.h"
#include "file_handler.h"
#include "warehouse.h"
using namespace std;

struct reservation_record {
    string order_id;
    int quantity;

    reservation_record(string id, int qty) {
        order_id = id;
        quantity = qty;
    }
};

struct rack_entry {
    string rack_id;
    string location_id;
    int total_quantity;
    int reserved_quantity;

    linkedList<reservation_record*> reservations;

    rack_entry(string rack, string location, int total, int reserved) {
        rack_id = rack;
        location_id = location;
        total_quantity = total;
        reserved_quantity = reserved;
    }

    int get_available_quantity() const {
        return total_quantity - reserved_quantity;
    }
};

struct avl_node {
    string product_id;
    string name;
    int unit_weight;
    int reorder_level;

    linkedList<rack_entry*> racks;

    int height;
    avl_node* left;
    avl_node* right;

    avl_node(string id, string n, int weight, int reorder) {
        product_id = id;
        name = n;
        unit_weight = weight;
        reorder_level = reorder;

        height = 1;
        left = nullptr;
        right = nullptr;
    }
};

class inventory {
private:
    avl_node* root;

    int get_height(avl_node* node);
    int get_balance(avl_node* node);

    avl_node* rotate_left(avl_node* node);
    avl_node* rotate_right(avl_node* node);

    avl_node* insert_node(avl_node* node, string id,
                          string name, int weight, int reorder);

    avl_node* delete_node(avl_node* node, string id);
    avl_node* detach_min(avl_node* node, avl_node*& min_node);
    avl_node* get_min(avl_node* node);

    void clear_tree(avl_node* node);
    void clear_racks(avl_node* node);

    bool reserve_on_rack(rack_entry* rack,
                         string order_id, int quantity);

    bool release_product_reservations(avl_node* product,
                                      string order_id);

    bool release_product_order(avl_node* product,
                               string order_id);

    bool deduct_from_rack(rack_entry* rack,
                          string order_id, int quantity);

    void display_inorder(avl_node* node);
    void display_reorder_inorder(avl_node* node);

public:
    inventory();
    ~inventory();

    void insert_product(string id, string name,
                        int weight, int reorder);

    avl_node* search_product(string id) const;

    bool delete_product(string id);

    bool add_rack(string product_id, string rack_id,
              string location_id, int quantity,
              warehouse& map);

    bool remove_rack(string product_id, string rack_id);

    bool update_quantity(string product_id, string rack_id,
                         int new_quantity);

    int get_total_stock(string product_id);
    int get_available_stock(string product_id);

    bool reserve_stock(string product_id, string order_id,
                       int quantity, string source_location,
                       warehouse& map);

    bool release_reservations(string order_id);

    bool deduct_reserved_stock(string product_id,
                               string order_id, int quantity);

    bool allocate_nearest_rack(string product_id, string order_id,
                               int quantity, string source_location,
                               warehouse& map);

    void display_products();
    void display_reorder_alerts();
    bool load_inventory(warehouse_data& data, warehouse& map);
    bool save_inventory(warehouse_data& data);
    void count_inventory(avl_node*, int&, int&, int&);
    void fill_inventory(avl_node*, warehouse_data&, int&, int&, int&);
};