#include "Inventory.h"
#include "file_handler.h"
#include <climits>

inventory::inventory() {
    root = nullptr;
}
static int max_value(int a, int b) {
    return (a > b) ? a : b;
}
inventory::~inventory() {
    clear_tree(root);
    root = nullptr;
}
// AVL HEIGHT AND BALANCE

int inventory::get_height(avl_node* node) {
    if (node == nullptr) {
        return 0;
    }

    return node->height;
}

int inventory::get_balance(avl_node* node) {
    if (node == nullptr) {
        return 0;
    }

    return get_height(node->left) - get_height(node->right);
}

// AVL ROTATIONS

avl_node* inventory::rotate_right(avl_node* node) {
    avl_node* left_child = node->left;
    avl_node* temp = left_child->right;

    left_child->right = node;
    node->left = temp;

    node->height = 1 + max_value(get_height(node->left),
                           get_height(node->right));

    left_child->height = 1 + max_value(get_height(left_child->left),
                                 get_height(left_child->right));

    return left_child;
}

avl_node* inventory::rotate_left(avl_node* node) {
    avl_node* right_child = node->right;
    avl_node* temp = right_child->left;

    right_child->left = node;
    node->right = temp;

    node->height = 1 + max_value(get_height(node->left),
                           get_height(node->right));

    right_child->height = 1 + max_value(get_height(right_child->left),
                                  get_height(right_child->right));

    return right_child;
}

// INSERT PRODUCT

avl_node* inventory::insert_node(avl_node* node, string id,
                                 string name, int weight, int reorder) {
    if (node == nullptr) {
        return new avl_node(id, name, weight, reorder);
    }

    if (id < node->product_id) {
        node->left = insert_node(node->left, id, name, weight, reorder);
    }
    else if (id > node->product_id) {
        node->right = insert_node(node->right, id, name, weight, reorder);
    }
    else {
        return node;
    }

    node->height = 1 + max_value(get_height(node->left),
                           get_height(node->right));

    int balance = get_balance(node);

    // LL
    if (balance > 1 && id < node->left->product_id) {
        return rotate_right(node);
    }

    // RR
    if (balance < -1 && id > node->right->product_id) {
        return rotate_left(node);
    }

    // LR
    if (balance > 1 && id > node->left->product_id) {
        node->left = rotate_left(node->left);
        return rotate_right(node);
    }

    // RL
    if (balance < -1 && id < node->right->product_id) {
        node->right = rotate_right(node->right);
        return rotate_left(node);
    }

    return node;
}

void inventory::insert_product(string id, string name,
                               int weight, int reorder) {
    if (id.empty() || name.empty() || weight <= 0 || reorder < 0) {
        cout << "Invalid product information.\n";
        return;
    }

    if (search_product(id) != nullptr) {
        cout << "Product already exists.\n";
        return;
    }

    root = insert_node(root, id, name, weight, reorder);
    cout << "Product inserted successfully.\n";
}

// SEARCH PRODUCT

avl_node* inventory::search_product(string id) const {
    avl_node* current = root;

    while (current != nullptr) {
        if (id == current->product_id) {
            return current;
        }

        if (id < current->product_id) {
            current = current->left;
        }
        else {
            current = current->right;
        }
    }

    return nullptr;
}

// FIND MINIMUM

avl_node* inventory::get_min(avl_node* node) {
    if (node == nullptr) {
        return nullptr;
    }

    while (node->left != nullptr) {
        node = node->left;
    }

    return node;
}

// DETACH MINIMUM NODE

avl_node* inventory::detach_min(avl_node* node,
                                avl_node*& min_node) {
    if (node->left == nullptr) {
        min_node = node;
        return node->right;
    }

    node->left = detach_min(node->left, min_node);

    node->height = 1 + max_value(get_height(node->left),
                           get_height(node->right));

    int balance = get_balance(node);

    if (balance > 1 && get_balance(node->left) >= 0) {
        return rotate_right(node);
    }

    if (balance > 1 && get_balance(node->left) < 0) {
        node->left = rotate_left(node->left);
        return rotate_right(node);
    }

    if (balance < -1 && get_balance(node->right) <= 0) {
        return rotate_left(node);
    }

    if (balance < -1 && get_balance(node->right) > 0) {
        node->right = rotate_right(node->right);
        return rotate_left(node);
    }

    return node;
}

// DELETE PRODUCT NODE

avl_node* inventory::delete_node(avl_node* node, string id) {
    if (node == nullptr) {
        return nullptr;
    }

    if (id < node->product_id) {
        node->left = delete_node(node->left, id);
    }
    else if (id > node->product_id) {
        node->right = delete_node(node->right, id);
    }
    else {
        if (node->left == nullptr || node->right == nullptr) {
            avl_node* child = nullptr;

            if (node->left != nullptr) {
                child = node->left;
            }
            else {
                child = node->right;
            }

            clear_racks(node);
            delete node;

            return child;
        }

        avl_node* successor = nullptr;

        avl_node* new_right = detach_min(node->right, successor);

        successor->left = node->left;
        successor->right = new_right;

        clear_racks(node);
        delete node;

        node = successor;
    }

    node->height = 1 + max_value(get_height(node->left),
                           get_height(node->right));

    int balance = get_balance(node);

    // LL
    if (balance > 1 && get_balance(node->left) >= 0) {
        return rotate_right(node);
    }

    // LR
    if (balance > 1 && get_balance(node->left) < 0) {
        node->left = rotate_left(node->left);
        return rotate_right(node);
    }

    // RR
    if (balance < -1 && get_balance(node->right) <= 0) {
        return rotate_left(node);
    }

    // RL
    if (balance < -1 && get_balance(node->right) > 0) {
        node->right = rotate_right(node->right);
        return rotate_left(node);
    }

    return node;
}

// DELETE PRODUCT

bool inventory::delete_product(string id) {
    avl_node* product = search_product(id);

    if (product == nullptr) {
        cout << "Product not found.\n";
        return false;
    }

    list_node<rack_entry*>* current = product->racks.head;

    while (current != nullptr) {
        if (current->data->reserved_quantity > 0) {
            cout << "Cannot delete product. Stock is reserved.\n";
            return false;
        }

        current = current->next;
    }

    root = delete_node(root, id);

    cout << "Product deleted successfully.\n";
    return true;
}

// RACK MANAGEMENT

bool inventory::add_rack(string product_id, string rack_id,
                         string location_id, int quantity,
                         warehouse& map) {
    avl_node* product = search_product(product_id);

    if (product == nullptr || rack_id.empty() ||
        location_id.empty() || quantity < 0) {
        return false;
    }

    location* loc = map.searchLocation(location_id);

    if (loc == nullptr || loc->type != "RACK") {
        cout << "Invalid rack location.\n";
        return false;
    }

    list_node<rack_entry*>* current = product->racks.head;

    while (current != nullptr) {
        if (current->data->rack_id == rack_id) {
            cout << "Rack already exists for this product.\n";
            return false;
        }

        current = current->next;
    }

    product->racks.insert_back(
        new rack_entry(rack_id, location_id, quantity, 0)
    );

    return true;
}

bool inventory::remove_rack(string product_id, string rack_id) {
    avl_node* product = search_product(product_id);

    if (product == nullptr) {
        return false;
    }

    list_node<rack_entry*>* previous = nullptr;
    list_node<rack_entry*>* current = product->racks.head;

    while (current != nullptr) {
        rack_entry* rack = current->data;

        if (rack->rack_id == rack_id) {
            if (rack->total_quantity != 0 ||
                rack->reserved_quantity != 0) {
                return false;
            }

            list_node<reservation_record*>* record =
                rack->reservations.head;

            while (record != nullptr) {
                delete record->data;
                record = record->next;
            }

            rack->reservations.clear();

            delete rack;
            product->racks.remove_node(previous, current);

            return true;
        }

        previous = current;
        current = current->next;
    }

    return false;
}

// UPDATE QUANTITY

bool inventory::update_quantity(string product_id, string rack_id,
                                int new_quantity) {
    avl_node* product = search_product(product_id);

    if (product == nullptr || new_quantity < 0) {
        return false;
    }

    list_node<rack_entry*>* current = product->racks.head;

    while (current != nullptr) {
        rack_entry* rack = current->data;

        if (rack->rack_id == rack_id) {
            if (new_quantity < rack->reserved_quantity) {
                cout << "Quantity cannot be less than reserved stock.\n";
                return false;
            }

            rack->total_quantity = new_quantity;
            return true;
        }

        current = current->next;
    }

    return false;
}

// STOCK CALCULATIONS

int inventory::get_total_stock(string product_id) {
    avl_node* product = search_product(product_id);

    if (product == nullptr) {
        return -1;
    }

    int total = 0;
    list_node<rack_entry*>* current = product->racks.head;

    while (current != nullptr) {
        total += current->data->total_quantity;
        current = current->next;
    }

    return total;
}

int inventory::get_available_stock(string product_id) {
    avl_node* product = search_product(product_id);

    if (product == nullptr) {
        return -1;
    }

    int total = 0;
    list_node<rack_entry*>* current = product->racks.head;

    while (current != nullptr) {
        total += current->data->get_available_quantity();
        current = current->next;
    }

    return total;
}

// RESERVE STOCK ON ONE RACK

bool inventory::reserve_on_rack(rack_entry* rack,
                                string order_id, int quantity) {
    if (rack == nullptr || quantity <= 0 ||
        rack->get_available_quantity() < quantity) {
        return false;
    }

    list_node<reservation_record*>* current =
        rack->reservations.head;

    while (current != nullptr) {
        if (current->data->order_id == order_id) {
            current->data->quantity += quantity;
            rack->reserved_quantity += quantity;
            return true;
        }

        current = current->next;
    }

    reservation_record* record =
        new reservation_record(order_id, quantity);

    rack->reservations.insert_back(record);
    rack->reserved_quantity += quantity;

    return true;
}

// RELEASE RESERVATIONS FOR ONE PRODUCT AND ORDER

bool inventory::release_product_order(avl_node* product,
                                      string order_id) {
    if (product == nullptr)
        return false;

    bool released = false;

    list_node<rack_entry*>* rack_node = product->racks.head;

    while (rack_node != nullptr) {
        rack_entry* rack = rack_node->data;

        list_node<reservation_record*>* previous = nullptr;
        list_node<reservation_record*>* current =
            rack->reservations.head;

        while (current != nullptr) {
            reservation_record* record = current->data;

            if (record->order_id == order_id) {
                rack->reserved_quantity -= record->quantity;

                delete record;
                rack->reservations.remove_node(previous, current);

                released = true;

                if (previous == nullptr)
                    current = rack->reservations.head;
                else
                    current = previous->next;
            }
            else {
                previous = current;
                current = current->next;
            }
        }

        rack_node = rack_node->next;
    }

    return released;
}

// RELEASE RESERVATIONS THROUGHOUT THE AVL TREE

bool inventory::release_product_reservations(avl_node* product,
                                             string order_id) {
    if (product == nullptr) {
        return false;
    }

    bool released = false;

    if (release_product_order(product, order_id)) {
        released = true;
    }

    if (release_product_reservations(product->left, order_id)) {
        released = true;
    }

    if (release_product_reservations(product->right, order_id)) {
        released = true;
    }

    return released;
}

bool inventory::release_reservations(string order_id) {
    if (order_id.empty()) {
        return false;
    }

    return release_product_reservations(root, order_id);
}

// DEDUCT STOCK AFTER ORDER FULFILLMENT

bool inventory::deduct_from_rack(rack_entry* rack,
                                 string order_id, int quantity) {
    if (rack == nullptr || quantity <= 0) {
        return false;
    }

    list_node<reservation_record*>* previous = nullptr;
    list_node<reservation_record*>* current =
        rack->reservations.head;

    while (current != nullptr) {
        reservation_record* record = current->data;

        if (record->order_id == order_id) {
            if (record->quantity < quantity) {
                return false;
            }

            record->quantity -= quantity;
            rack->reserved_quantity -= quantity;
            rack->total_quantity -= quantity;

            if (record->quantity == 0) {
                delete record;
                rack->reservations.remove_node(previous, current);
            }

            return true;
        }

        previous = current;
        current = current->next;
    }

    return false;
}

// DEDUCT RESERVED STOCK FOR A PRODUCT

bool inventory::deduct_reserved_stock(string product_id,
                                      string order_id, int quantity) {
    avl_node* product = search_product(product_id);

    if (product == nullptr || quantity <= 0) {
        return false;
    }

    int available_reserved = 0;

    list_node<rack_entry*>* rack_node = product->racks.head;

    while (rack_node != nullptr) {
        list_node<reservation_record*>* record =
            rack_node->data->reservations.head;

        while (record != nullptr) {
            if (record->data->order_id == order_id) {
                available_reserved += record->data->quantity;
            }

            record = record->next;
        }

        rack_node = rack_node->next;
    }

    if (available_reserved < quantity) {
        return false;
    }

    int remaining = quantity;
    rack_node = product->racks.head;

    while (rack_node != nullptr && remaining > 0) {
        rack_entry* rack = rack_node->data;

        list_node<reservation_record*>* previous = nullptr;
        list_node<reservation_record*>* current =
            rack->reservations.head;

        while (current != nullptr && remaining > 0) {
            reservation_record* record = current->data;

            if (record->order_id == order_id) {
                int amount = record->quantity;

                if (amount > remaining) {
                    amount = remaining;
                }

                record->quantity -= amount;
                rack->reserved_quantity -= amount;
                rack->total_quantity -= amount;
                remaining -= amount;

                if (record->quantity == 0) {
                    list_node<reservation_record*>* next =
                        current->next;

                    rack->reservations.remove_node(previous, current);
                    delete record;

                    current = next;
                    continue;
                }
            }

            previous = current;
            current = current->next;
        }

        rack_node = rack_node->next;
    }

    return remaining == 0;
}

// ALLOCATE STOCK FROM NEAREST REACHABLE RACKS

bool inventory::allocate_nearest_rack(string product_id,
                                      string order_id,
                                      int quantity,
                                      string source_location,
                                      warehouse& map) {
    avl_node* product = search_product(product_id);

    if (product == nullptr || order_id.empty() ||
        source_location.empty() || quantity <= 0) {
        return false;
    }

    if (get_available_stock(product_id) < quantity) {
        cout << "Not enough available stock.\n";
        return false;
    }

    // Prevent duplicate reservation of this product for this order.
    list_node<rack_entry*>* check_rack = product->racks.head;

    while (check_rack != nullptr) {
        list_node<reservation_record*>* check_record =
            check_rack->data->reservations.head;

        while (check_record != nullptr) {
            if (check_record->data->order_id == order_id) {
                cout << "This order already has a reservation for this product.\n";
                return false;
            }

            check_record = check_record->next;
        }

        check_rack = check_rack->next;
    }

    // Validate source location.
    if (map.searchLocation(source_location) == nullptr) {
        cout << "Source location does not exist.\n";
        return false;
    }

    int remaining = quantity;

    // Keep selecting the closest reachable rack.
    while (remaining > 0) {
        rack_entry* nearest_rack = nullptr;
        int shortest_distance = INT_MAX;

        list_node<rack_entry*>* current = product->racks.head;

        while (current != nullptr) {
            rack_entry* rack = current->data;

            if (rack->get_available_quantity() > 0) {
                location* rack_location =
                    map.searchLocation(rack->location_id);

                if (rack_location != nullptr &&
                    rack_location->type == "RACK") {

                    linkedList<string> route;

                    int distance = map.findShortestRoute(
                        source_location,
                        rack->location_id,
                        route
                    );

                    route.clear();

                    if (distance != -1) {
                        if (nearest_rack == nullptr ||
                            distance < shortest_distance ||
                            (distance == shortest_distance &&
                             rack->rack_id < nearest_rack->rack_id)) {

                            nearest_rack = rack;
                            shortest_distance = distance;
                        }
                    }
                }
            }

            current = current->next;
        }

        // No reachable rack has available stock.
        if (nearest_rack == nullptr) {
            release_product_order(product, order_id);
            cout << "Could not allocate all stock. Reservation cancelled.\n";
            return false;
        }

        int amount = nearest_rack->get_available_quantity();

        if (amount > remaining) {
            amount = remaining;
        }

        if (!reserve_on_rack(nearest_rack, order_id, amount)) {
            release_product_order(product, order_id);
            return false;
        }

        remaining -= amount;
    }

    return true;
}

// SAME OPERATION WITH A SHORTER NAME

bool inventory::reserve_stock(string product_id, string order_id,
                              int quantity, string source_location,
                              warehouse& map) {
    return allocate_nearest_rack(product_id, order_id, quantity,
                                 source_location, map);
}

// DISPLAY PRODUCTS

void inventory::display_inorder(avl_node* node) {
    if (node == nullptr) {
        return;
    }

    display_inorder(node->left);

    cout << "\nProduct ID: " << node->product_id << endl;
    cout << "Name: " << node->name << endl;
    cout << "Unit Weight: " << node->unit_weight << endl;
    cout << "Reorder Level: " << node->reorder_level << endl;

    cout << "Racks:\n";

    list_node<rack_entry*>* current = node->racks.head;

    if (current == nullptr) {
        cout << "  No racks assigned.\n";
    }

    while (current != nullptr) {
        rack_entry* rack = current->data;

        cout << "  Rack: " << rack->rack_id
             << " | Location: " << rack->location_id
             << " | Total: " << rack->total_quantity
             << " | Reserved: " << rack->reserved_quantity
             << " | Available: " << rack->get_available_quantity()
             << endl;

        current = current->next;
    }

    display_inorder(node->right);
}

void inventory::display_products() {
    if (root == nullptr) {
        cout << "Inventory is empty.\n";
        return;
    }

    display_inorder(root);
}

// REORDER ALERTS

void inventory::display_reorder_inorder(avl_node* node) {
    if (node == nullptr) {
        return;
    }

    display_reorder_inorder(node->left);

    int available = get_available_stock(node->product_id);

    if (available <= node->reorder_level) {
        cout << "Product: " << node->product_id
             << " | Name: " << node->name
             << " | Available: " << available
             << " | Reorder Level: " << node->reorder_level
             << endl;
    }

    display_reorder_inorder(node->right);
}

void inventory::display_reorder_alerts() {
    if (root == nullptr) {
        cout << "Inventory is empty.\n";
        return;
    }

    cout << "\nProducts Requiring Reorder:\n";
    display_reorder_inorder(root);
}

// MEMORY CLEANUP

void inventory::clear_racks(avl_node* node) {
    if (node == nullptr) {
        return;
    }

    list_node<rack_entry*>* rack_node = node->racks.head;

    while (rack_node != nullptr) {
        rack_entry* rack = rack_node->data;

        list_node<reservation_record*>* record =
            rack->reservations.head;

        while (record != nullptr) {
            delete record->data;
            record = record->next;
        }

        rack->reservations.clear();

        delete rack;
        rack_node = rack_node->next;
    }

    node->racks.clear();
}

void inventory::clear_tree(avl_node* node) {
    if (node == nullptr) {
        return;
    }

    clear_tree(node->left);
    clear_tree(node->right);

    clear_racks(node);

    delete node;
}


// for invenotry.cpp
bool inventory::load_inventory(warehouse_data& data, warehouse& map) {
    if (!load_products(data) || !load_racks(data) ||
        !load_reservations(data)) {
        cout << "Error loading inventory files.\n";
        return false;
    }

    // Load products into the AVL tree.
    for (int i = 0; i < data.product_count; i++) {
        insert_product(
            data.products[i].product_id,
            data.products[i].name,
            data.products[i].unit_weight,
            data.products[i].reorder_level
        );
    }

    // Load rack entries.
    for (int i = 0; i < data.rack_count; i++) {
        rack_data& r = data.racks[i];

        if (!add_rack(r.product_id, r.rack_id,
                      r.location_id, r.total_quantity, map)) {
            cout << "Could not load rack " << r.rack_id << ".\n";
            continue;
        }

        avl_node* product = search_product(r.product_id);
        list_node<rack_entry*>* current = product->racks.head;

        while (current != nullptr) {
            if (current->data->rack_id == r.rack_id) {
                current->data->reserved_quantity = 0;
                break;
            }
            current = current->next;
        }
        cout << "Inventory loaded.\n";
        return true;
    }

    // Restore reservation records.
    for (int i = 0; i < data.reservation_count; i++) {
        reservation_data& r = data.reservations[i];

        avl_node* product = search_product(r.product_id);

        if (product == nullptr) {
            cout << "Reservation has an unknown product: "
                 << r.product_id << endl;
            continue;
        }

        list_node<rack_entry*>* current = product->racks.head;

        while (current != nullptr) {
            rack_entry* rack = current->data;

            if (rack->rack_id == r.rack_id) {
                if (r.quantity > 0 &&
                    rack->reserved_quantity + r.quantity <=
                    rack->total_quantity) {

                    rack->reservations.insert_back(
                        new reservation_record(r.order_id, r.quantity)
                    );

                    rack->reserved_quantity += r.quantity;
                }
                else {
                    cout << "Invalid reservation for rack "
                         << r.rack_id << ".\n";
                }

                break;
            }

            current = current->next;
        }
    }

    cout << "Inventory loaded.\n";
    return true;
}

void inventory::count_inventory(avl_node* node,
    int& products, int& racks, int& reservations) {

    if (node == nullptr) {
        return;
    }

    products++;

    list_node<rack_entry*>* r = node->racks.head;

    while (r != nullptr) {
        racks++;

        list_node<reservation_record*>* res =
            r->data->reservations.head;

        while (res != nullptr) {
            reservations++;
            res = res->next;
        }

        r = r->next;
    }

    count_inventory(node->left, products, racks, reservations);
    count_inventory(node->right, products, racks, reservations);
}

void inventory::fill_inventory(avl_node* node,
    warehouse_data& data, int& p, int& r, int& res) {

    if (node == nullptr) {
        return;
    }

    data.products[p].product_id = node->product_id;
    data.products[p].name = node->name;
    data.products[p].unit_weight = node->unit_weight;
    data.products[p].reorder_level = node->reorder_level;
    p++;

    list_node<rack_entry*>* current = node->racks.head;

    while (current != nullptr) {
        rack_entry* rack = current->data;

        data.racks[r].rack_id = rack->rack_id;
        data.racks[r].location_id = rack->location_id;
        data.racks[r].product_id = node->product_id;
        data.racks[r].total_quantity = rack->total_quantity;
        data.racks[r].reserved_quantity = rack->reserved_quantity;
        r++;

        list_node<reservation_record*>* record =
            rack->reservations.head;

        while (record != nullptr) {
            data.reservations[res].order_id = record->data->order_id;
            data.reservations[res].product_id = node->product_id;
            data.reservations[res].rack_id = rack->rack_id;
            data.reservations[res].quantity = record->data->quantity;
            res++;

            record = record->next;
        }

        current = current->next;
    }

    fill_inventory(node->left, data, p, r, res);
    fill_inventory(node->right, data, p, r, res);
}

bool inventory::save_inventory(warehouse_data& data) {
    int products = 0;
    int racks = 0;
    int reservations = 0;

    count_inventory(root, products, racks, reservations);

    product_data* new_products = nullptr;
    rack_data* new_racks = nullptr;
    reservation_data* new_reservations = nullptr;

    if (products > 0)
        new_products = new product_data[products];

    if (racks > 0)
        new_racks = new rack_data[racks];

    if (reservations > 0)
        new_reservations = new reservation_data[reservations];

    delete[] data.products;
    delete[] data.racks;
    delete[] data.reservations;

    data.products = new_products;
    data.racks = new_racks;
    data.reservations = new_reservations;

    data.product_count = products;
    data.rack_count = racks;
    data.reservation_count = reservations;

    data.product_capacity = products;
    data.rack_capacity = racks;
    data.reservation_capacity = reservations;

    int p = 0, r = 0, res = 0;

    fill_inventory(root, data, p, r, res);

    bool a = save_products(data);
    bool b = save_racks(data);
    bool c = save_reservations(data);

    return a && b && c;
}