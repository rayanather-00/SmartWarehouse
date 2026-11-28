#include "file_handler.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
using namespace std;

// Dynamically loads records and allocates enough memory for all records
template <typename T, typename Parser>
bool load_records(const char* filename, T*& records, int& count, int& capacity, Parser parse) {
    ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    int new_capacity = 10;
    int new_count = 0;
    T* new_records = new T[new_capacity];

    string line;

    while (getline(file, line)) {
        T record;
        if (!parse(line, record)) {
            continue;
        }
        if (new_count >= new_capacity) {
            new_capacity *= 2;

            T* temp = new T[new_capacity];

            for (int i = 0; i < new_count; i++) {
                temp[i] = new_records[i];
            }

            delete[] new_records;
            new_records = temp;
        }

        new_records[new_count] = record;
        new_count++;
    }

    file.close();

    delete[] records;

    records = new_records;
    count = new_count;
    capacity = new_capacity;

    return true;
}


// Format: LocationID,Type,Name
bool load_locations(warehouse_data& data) {
    return load_records("locations.txt", data.locations, data.location_count, data.location_capacity,
        [](const string& line, location_data& record) {
            stringstream ss(line);
            string id, type, name;

            getline(ss, id, ',');
            getline(ss, type, ',');
            getline(ss, name);

            if (id.empty()) {
                return false;
            }

            record.location_id = id;
            record.type = type;
            record.name = name;

            return true;
        });
}


// file format Location1,Location2,Distance,Status
// lines represents a connection between two locations
// its distance/weight and whether the pathway is OPEN or BLOCKED
bool load_pathways(warehouse_data& data) {
    return load_records("pathways.txt", data.pathways, data.pathway_count, data.pathway_capacity,
        [](const string& line, pathway_data& record) {
            stringstream ss(line);
            string loc1, loc2, distance, status;

            getline(ss, loc1, ',');
            getline(ss, loc2, ',');
            getline(ss, distance, ',');
            getline(ss, status);

            if (loc1.empty()) {
                return false;
            }

            record.location1 = loc1;
            record.location2 = loc2;
            record.distance = atoi(distance.c_str());
            record.status = status;

            return true;
        });
}


// file format ProductID,Name,UnitWeight,ReorderLevel
// lines contains a product ID, name, weight and reorder level
bool load_products(warehouse_data& data) {
    return load_records("products.txt", data.products, data.product_count, data.product_capacity,
        [](const string& line, product_data& record) {
            stringstream ss(line);
            string id, name, weight, reorder;

            getline(ss, id, ',');
            getline(ss, name, ',');
            getline(ss, weight, ',');
            getline(ss, reorder);

            if (id.empty()) {
                return false;
            }

            record.product_id = id;
            record.name = name;
            record.unit_weight = atoi(weight.c_str());
            record.reorder_level = atoi(reorder.c_str());

            return true;
        });
}


// format RackID,LocationID,ProductID,TotalQuantity,ReservedQuantity
// lines connects a rack with a location and product including its total and reserved stock
bool load_racks(warehouse_data& data) {
    return load_records("racks.txt", data.racks, data.rack_count, data.rack_capacity,
        [](const string& line, rack_data& record) {
            stringstream ss(line);
            string rack, location, product, total, reserved;

            getline(ss, rack, ',');
            getline(ss, location, ',');
            getline(ss, product, ',');
            getline(ss, total, ',');
            getline(ss, reserved);

            if (rack.empty()) {
                return false;
            }

            record.rack_id = rack;
            record.location_id = location;
            record.product_id = product;
            record.total_quantity = atoi(total.c_str());
            record.reserved_quantity = atoi(reserved.c_str());

            return true;
        });
}


// formt ChargingStationID,LocationID,TotalSlots
// lines represents a charging station, its location and the number of available charging slots
bool load_chargers(warehouse_data& data) {
    return load_records("chargers.txt", data.chargers, data.charger_count, data.charger_capacity,
        [](const string& line, charger_data& record) {
            stringstream ss(line);
            string id, location, slots;

            getline(ss, id, ',');
            getline(ss, location, ',');
            getline(ss, slots);

            if (id.empty()) {
                return false;
            }

            record.charging_station_id = id;
            record.location_id = location;
            record.total_slots = atoi(slots.c_str());

            return true;
        });
}


// format RobotID,Type,CurrentLocation,Battery,MaxCapacity,Status,AssignedTask
// line contains one robot and its current operating information
bool load_robots(warehouse_data& data) {
    return load_records("robots.txt", data.robots, data.robot_count, data.robot_capacity,
        [](const string& line, robot_data& record) {
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
                return false;
            }

            record.robot_id = id;
            record.type = type;
            record.current_location = location;
            record.battery = atof(battery.c_str());
            record.max_capacity = atoi(capacity.c_str());
            record.status = status;
            record.assigned_task = task;

            return true;
        });
}


// format OrderID,CustomerID,Status,ProductID:Quantity|ProductID:Quantity
// lines contains an order. The final field stores one or more
// product IDs and quantities, separated by |.
bool load_orders(warehouse_data& data) {
    return load_records("orders.txt", data.orders, data.order_count, data.order_capacity,
        [](const string& line, order_data& record) {
            stringstream ss(line);
            string id, customer, status, products;

            getline(ss, id, ',');
            getline(ss, customer, ',');
            getline(ss, status, ',');
            getline(ss, products);

            if (id.empty()) {
                return false;
            }

            record.order_id = id;
            record.customer_id = customer;
            record.status = status;
            record.products = products;

            return true;
        });
}


// Calls all seven loading functions and if any not open it will return false
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
        file << data.products[i].product_id << "," << data.products[i].name << "," << data.products[i].unit_weight << "," << data.products[i].reorder_level << endl;
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
        file << data.racks[i].rack_id << "," << data.racks[i].location_id << "," << data.racks[i].product_id << "," << data.racks[i].total_quantity << "," << data.racks[i].reserved_quantity << endl;
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
        file << data.chargers[i].charging_station_id << "," << data.chargers[i].location_id << "," << data.chargers[i].total_slots << endl;
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
        file << data.robots[i].robot_id << "," << data.robots[i].type << "," << data.robots[i].current_location << "," << data.robots[i].battery << ","
             << data.robots[i].max_capacity << "," << data.robots[i].status << "," << data.robots[i].assigned_task << endl;
    }

    file.close();
    return true;
}


bool save_orders(const warehouse_data& data) {
    ofstream file("orders.txt");
    if (!file.is_open()) {
        return false;
    }

    for (int i = 0; i < data.order_count; i++) {
        file << data.orders[i].order_id << "," << data.orders[i].customer_id << "," << data.orders[i].status << "," << data.orders[i].products << endl;
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


// Releases all dynamically allocated memory
void clear_warehouse_data(warehouse_data& data) {
    delete[] data.locations;
    delete[] data.pathways;
    delete[] data.products;
    delete[] data.racks;
    delete[] data.chargers;
    delete[] data.robots;
    delete[] data.orders;

    data.locations = nullptr;
    data.pathways = nullptr;
    data.products = nullptr;
    data.racks = nullptr;
    data.chargers = nullptr;
    data.robots = nullptr;
    data.orders = nullptr;

    data.location_count = 0;
    data.pathway_count = 0;
    data.product_count = 0;
    data.rack_count = 0;
    data.charger_count = 0;
    data.robot_count = 0;
    data.order_count = 0;

    data.location_capacity = 0;
    data.pathway_capacity = 0;
    data.product_capacity = 0;
    data.rack_capacity = 0;
    data.charger_capacity = 0;
    data.robot_capacity = 0;
    data.order_capacity = 0;
    // for invenotry.cpp
    delete[] data.reservations;
    data.reservations = nullptr;
    data.reservation_count = 0;
    data.reservation_capacity = 0;
}


//for inventory.cpp
bool load_reservations(warehouse_data& data) {
    return load_records("reservations.txt",
        data.reservations,
        data.reservation_count,
        data.reservation_capacity,
        [](const string& line, reservation_data& record) {
            stringstream ss(line);

            string order, product, rack, quantity;

            getline(ss, order, ',');
            getline(ss, product, ',');
            getline(ss, rack, ',');
            getline(ss, quantity);

            if (order.empty() || product.empty() ||
                rack.empty() || quantity.empty()) {
                return false;
            }

            int qty = atoi(quantity.c_str());

            if (qty <= 0) {
                return false;
            }

            record.order_id = order;
            record.product_id = product;
            record.rack_id = rack;
            record.quantity = qty;

            return true;
        });
}

bool save_reservations(const warehouse_data& data) {
    ofstream file("reservations.txt");

    if (!file.is_open()) {
        return false;
    }

    for (int i = 0; i < data.reservation_count; i++) {
        file << data.reservations[i].order_id << ","
             << data.reservations[i].product_id << ","
             << data.reservations[i].rack_id << ","
             << data.reservations[i].quantity << endl;
    }

    file.close();
    return true;
}