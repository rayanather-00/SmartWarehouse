#include "file_handler.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
using namespace std;


// Format: LocationID,Type,Name
bool load_locations(warehouse_data& data) {
    ifstream file("locations.txt");
    if (!file.is_open()) {
        return false;
    }
    string line;
    data.location_count = 0;
    while (getline(file, line) && data.location_count < MAX_LOCATIONS) {
        stringstream ss(line);
        string id, type, name;
        getline(ss, id, ',');
        getline(ss, type, ',');
        getline(ss, name);
        if (id.empty()) {
            continue;
        }
        int i = data.location_count;
        data.locations[i].location_id = id;
        data.locations[i].type = type;
        data.locations[i].name = name;
        data.location_count++;
    }

    file.close();
    return true;
}

// file format Location1,Location2,Distance,Status
// lines represents a connection between two locations
// its distance/weight and whether the pathway is OPEN or BLOCKED
bool load_pathways(warehouse_data& data) {
    ifstream file("pathways.txt");
    if (!file.is_open()) {
        return false;
    }
    string line;
    data.pathway_count = 0;
    while (getline(file, line) && data.pathway_count < MAX_PATHWAYS) {
        stringstream ss(line);
        string loc1, loc2, distance, status;
        getline(ss, loc1, ',');
        getline(ss, loc2, ',');
        getline(ss, distance, ',');
        getline(ss, status);
        if (loc1.empty()) {
            continue;
        }
        int i = data.pathway_count;
        data.pathways[i].location1 = loc1;
        data.pathways[i].location2 = loc2;
        data.pathways[i].distance = atoi(distance.c_str());
        data.pathways[i].status = status;
        data.pathway_count++;
    }
    file.close();
    return true;
}

// file format ProductID,Name,UnitWeight,ReorderLevel
// lines contains a product ID, name, weight and reorder level
bool load_products(warehouse_data& data) {
    ifstream file("products.txt");
    if (!file.is_open()) {
        return false;
    }
    string line;
    data.product_count = 0;
    while (getline(file, line) && data.product_count < MAX_PRODUCTS) {
        stringstream ss(line);
        string id, name, weight, reorder;
        getline(ss, id, ',');
        getline(ss, name, ',');
        getline(ss, weight, ',');
        getline(ss, reorder);
        if (id.empty()) {
            continue;
        }
        int i = data.product_count;
        data.products[i].product_id = id;
        data.products[i].name = name;
        data.products[i].unit_weight = atoi(weight.c_str());
        data.products[i].reorder_level = atoi(reorder.c_str());
        data.product_count++;
    }
    file.close();
    return true;
}

// format RackID,LocationID,ProductID,TotalQuantity,ReservedQuantity
// lines connects a rack with a location and product including its total and reserved stock
bool load_racks(warehouse_data& data) {
    ifstream file("racks.txt");
    if (!file.is_open()) {
        return false;
    }
    string line;
    data.rack_count = 0;
    while (getline(file, line) && data.rack_count < MAX_RACKS) {
        stringstream ss(line);
        string rack, location, product, total, reserved;
        getline(ss, rack, ',');
        getline(ss, location, ',');
        getline(ss, product, ',');
        getline(ss, total, ',');
        getline(ss, reserved);
        if (rack.empty()) {
            continue;
        }
        int i = data.rack_count;
        data.racks[i].rack_id = rack;
        data.racks[i].location_id = location;
        data.racks[i].product_id = product;
        data.racks[i].total_quantity = atoi(total.c_str());
        data.racks[i].reserved_quantity = atoi(reserved.c_str());
        data.rack_count++;
    }
    file.close();
    return true;
}

// formt ChargingStationID,LocationID,TotalSlots
// lines represents a charging station, its location and the number of available charging slots
bool load_chargers(warehouse_data& data) {
    ifstream file("chargers.txt");
    if (!file.is_open()) {
        return false;
    }
    string line;
    data.charger_count = 0;
    while (getline(file, line) && data.charger_count < MAX_CHARGERS) {
        stringstream ss(line);
        string id, location, slots;
        getline(ss, id, ',');
        getline(ss, location, ',');
        getline(ss, slots);
        if (id.empty()) {
            continue;
        }
        int i = data.charger_count;
        data.chargers[i].charging_station_id = id;
        data.chargers[i].location_id = location;
        data.chargers[i].total_slots = atoi(slots.c_str());
        data.charger_count++;
    }
    file.close();
    return true;
}

// format RobotID,Type,CurrentLocation,Battery,MaxCapacity,Status,AssignedTask
//line contains one robot and its current operating information
bool load_robots(warehouse_data& data) {
    ifstream file("robots.txt");
    if (!file.is_open()) {
        return false;
    }
    string line;
    data.robot_count = 0;
    while (getline(file, line) && data.robot_count < MAX_ROBOTS) {
        stringstream ss(line);
        string id, type, location, battery, capacity, status, task;
        getline(ss, id, ',');
        getline(ss, type, ',');
        getline(ss, location, ',');
        getline(ss, battery, ',');
        getline(ss, capacity, ',');
        getline(ss, status, ',');
        getline(ss, task);
        if (id.empty()) {
            continue;
        }
        int i = data.robot_count;
        data.robots[i].robot_id = id;
        data.robots[i].type = type;
        data.robots[i].current_location = location;
        data.robots[i].battery = atoi(battery.c_str());
        data.robots[i].max_capacity = atoi(capacity.c_str());
        data.robots[i].status = status;
        data.robots[i].assigned_task = task;
        data.robot_count++;
    }
    file.close();
    return true;
}

// format OrderID,CustomerID,Status,ProductID:Quantity|ProductID:Quantity
// lines contains an order. The final field stores one or more
// product IDs and quantities, separated by |.
bool load_orders(warehouse_data& data) {
    ifstream file("orders.txt");
    if (!file.is_open()) {
        return false;
    }
    string line;
    data.order_count = 0;
    while (getline(file, line) && data.order_count < MAX_ORDERS) {
        stringstream ss(line);
        string id, customer, status, products;
        getline(ss, id, ',');
        getline(ss, customer, ',');
        getline(ss, status, ',');
        getline(ss, products);
        if (id.empty()) {
            continue;
        }
        int i = data.order_count;
        data.orders[i].order_id = id;
        data.orders[i].customer_id = customer;
        data.orders[i].status = status;
        data.orders[i].products = products;
        data.order_count++;
    }
    file.close();
    return true;
}

// Calls all seven loading functions and if any not open it will retunr false 
bool load_all_data(warehouse_data& data) {
    bool a = load_locations(data);
    bool b = load_pathways(data);
    bool c = load_products(data);
    bool d = load_racks(data);
    bool e = load_chargers(data);
    bool f = load_robots(data);
    bool g = load_orders(data);
    return (a && b && c && d && e && f && g);
}

// Writes locations.txt in the same comma-separated format.
bool save_locations(const warehouse_data& data) {
    ofstream file("locations.txt");
    if (!file.is_open()) {
        return false;
    }
    for (int i = 0; i < data.location_count; i++) {
        file << data.locations[i].location_id << "," << data.locations[i].type << "," << data.locations[i].name << endl;
    }
    file.close();
    return true;
}

bool save_pathways(const warehouse_data& data) {
    ofstream file("pathways.txt");
    if (!file.is_open()) {
        return false;
    }
    for (int i = 0; i < data.pathway_count; i++) {
        file << data.pathways[i].location1 << "," << data.pathways[i].location2 << "," << data.pathways[i].distance << "," << data.pathways[i].status << endl;
    }
    file.close();
    return true;
}

bool save_products(const warehouse_data& data) {
    ofstream file("products.txt");
    if (!file.is_open()) {
        return false;
    }
    for (int i = 0; i < data.product_count; i++) {
        file << data.products[i].product_id << "," << data.products[i].name << ","<< data.products[i].unit_weight << "," << data.products[i].reorder_level << endl;
    }
    file.close();
    return true;
}

bool save_racks(const warehouse_data& data) {
    ofstream file("racks.txt");
    if (!file.is_open()) {
        return false;
    }
    for (int i = 0; i < data.rack_count; i++) {
        file << data.racks[i].rack_id << ","  << data.racks[i].location_id << "," << data.racks[i].product_id << ","  << data.racks[i].total_quantity << "," << data.racks[i].reserved_quantity << endl;
    }
    file.close();
    return true;
}

bool save_chargers(const warehouse_data& data) {
    ofstream file("chargers.txt");
    if (!file.is_open()) {
        return false;
    }
    for (int i = 0; i < data.charger_count; i++) {
        file << data.chargers[i].charging_station_id << ","  << data.chargers[i].location_id << ","  << data.chargers[i].total_slots << endl;
    }
    file.close();
    return true;
}

bool save_robots(const warehouse_data& data) {
    ofstream file("robots.txt");
    if (!file.is_open()) {
        return false;
    }
    for (int i = 0; i < data.robot_count; i++) { //it is toooo dam long
        file << data.robots[i].robot_id << "," << data.robots[i].type << ","  << data.robots[i].current_location << ","  << data.robots[i].battery << ","
             << data.robots[i].max_capacity << "," << data.robots[i].status << ","  << data.robots[i].assigned_task << endl;
    }
    file.close();
    return true;
}


bool save_orders(const warehouse_data& data) {
    ofstream file("orders.txt");
    if (!file.is_open())  {
        return false;
    }
    for (int i = 0; i < data.order_count; i++)
    {
        file << data.orders[i].order_id << "," << data.orders[i].customer_id << "," << data.orders[i].status << ","<< data.orders[i].products << endl;
    }
    file.close();
    return true;
}

// calls all seven saving functions returns false if any file cannot be written
bool save_all_data(const warehouse_data& data) {
    bool a = save_locations(data);
    bool b = save_pathways(data);
    bool c = save_products(data);
    bool d = save_racks(data);
    bool e = save_chargers(data);
    bool f = save_robots(data);
    bool g = save_orders(data);

    return (a && b && c && d && e && f && g);
}
