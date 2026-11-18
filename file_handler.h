#pragma once 
#include <string>
using namespace std;

// this is manimum number of recod can stored in a file (change value if u want)
// lowkey now i am thinking we can use dynamic too (these all are msg for abdul)
const int MAX_LOCATIONS = 100;
const int MAX_PATHWAYS = 200;
const int MAX_PRODUCTS = 100;
const int MAX_RACKS = 200;
const int MAX_CHARGERS = 50;
const int MAX_ROBOTS = 50;
const int MAX_ORDERS = 200;

class location_data {
public:
    string location_id;
    string type;
    string name;
};

class pathway_data {
    public:
    string location1;
    string location2;
    int distance;
    string status;
};

class product_data {
    public:
    string product_id;
    string name;
    int unit_weight;
    int reorder_level;
};

class rack_data {
    public:
    string rack_id;
    string location_id;
    string product_id;
    int total_quantity;
    int reserved_quantity;
};

class charger_data {
    public:
    string charging_station_id;
    string location_id;
    int total_slots;
};

class robot_data {
    public:
    string robot_id;
    string type;
    string current_location;
    int battery;
    int max_capacity;
    string status;
    string assigned_task;
};

class order_data {
    public:
    string order_id;
    string customer_id;
    string status;
    string products; // Stores the final field as written in orders.txt like-> P101:5|P102:3
};


class warehouse_data { //home for everything
    public:
    location_data locations[MAX_LOCATIONS];
    pathway_data pathways[MAX_PATHWAYS];
    product_data products[MAX_PRODUCTS];
    rack_data racks[MAX_RACKS];
    charger_data chargers[MAX_CHARGERS];
    robot_data robots[MAX_ROBOTS];
    order_data orders[MAX_ORDERS];
    int location_count;
    int pathway_count;
    int product_count;
    int rack_count;
    int charger_count;
    int robot_count;
    int order_count;
};


bool load_locations(warehouse_data& data);
bool load_pathways(warehouse_data& data);
bool load_products(warehouse_data& data);
bool load_racks(warehouse_data& data);
bool load_chargers(warehouse_data& data);
bool load_robots(warehouse_data& data);
bool load_orders(warehouse_data& data);
bool save_locations(const warehouse_data& data);
bool save_pathways(const warehouse_data& data);
bool save_products(const warehouse_data& data);
bool save_racks(const warehouse_data& data);
bool save_chargers(const warehouse_data& data);
bool save_robots(const warehouse_data& data);
bool save_orders(const warehouse_data& data);

// these two same thing but check all seven at the same time
bool save_all_data(const warehouse_data& data);
bool load_all_data(warehouse_data& data);

