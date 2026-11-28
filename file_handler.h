#pragma once
#include <string>
using namespace std;

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
  double battery;
  int max_capacity;
  string status;
  string assigned_task;
};

class order_data {
 public:
  string order_id;
  string customer_id;
  string status;
  string products;  // Stores the final field as written in orders.txt like-> P101:5|P102:3
};



class warehouse_data {  // home for everything
 public:
  location_data* locations = nullptr;
  pathway_data* pathways = nullptr;
  product_data* products = nullptr;
  rack_data* racks = nullptr;
  charger_data* chargers = nullptr;
  robot_data* robots = nullptr;
  order_data* orders = nullptr;

  int location_count = 0;
  int pathway_count = 0;
  int product_count = 0;
  int rack_count = 0;
  int charger_count = 0;
  int robot_count = 0;
  int order_count = 0;

  int location_capacity = 0;
  int pathway_capacity = 0;
  int product_capacity = 0;
  int rack_capacity = 0;
  int charger_capacity = 0;
  int robot_capacity = 0;
  int order_capacity = 0;
};

void clear_warehouse_data(warehouse_data& data);

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
