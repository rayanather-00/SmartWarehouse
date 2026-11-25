#pragma once
#include <string>

#include "LinkedList.h"
#include "warehouse.h"

using namespace std;

class robot {
 public:
  string id;
  string type;
  string current_location;
  double battery;
  int max_capacity;
  string status;
  string assigned_task;

  robot() {
    battery = 0.0;
    max_capacity = 0;
    status = "Available";
    assigned_task = "";
  }

  virtual ~robot() {}
  virtual double get_battery_rate() const = 0;
  virtual bool is_valid_capacity() const = 0;
  virtual bool is_eligible_for_order(int total_order_weight, int max_item_weight) const = 0;
};

class picker : public robot {
 public:
  double get_battery_rate() const {
    return 0.20;
  }

  bool is_valid_capacity() const {
    return max_capacity > 0 && max_capacity <= 15;
  }

  bool is_eligible_for_order(int total_order_weight, int max_item_weight) const {
    return max_item_weight <= 5;
  }
};

class carrier : public robot {
 public:
  double get_battery_rate() const {
    return 0.25;
  }

  bool is_valid_capacity() const {
    return max_capacity >= 15 && max_capacity <= 30;
  }

  bool is_eligible_for_order(int total_order_weight, int max_item_weight) const {
    return true;
  }
};

class heavy_duty : public robot {
 public:
  double get_battery_rate() const {
    return 0.35;
  }

  bool is_valid_capacity() const {
    return max_capacity > 0 && max_capacity <= 60;
  }

  bool is_eligible_for_order(int total_order_weight, int max_item_weight) const {
    return total_order_weight > 15 || max_item_weight > 10;
  }
};

class fleet_manager {
 public:
  linkedList<robot*> robots;

  void loadRobots(const warehouse& w);
  bool registerRobot(string id, string type, string location, double battery, int max_capacity, string status, string task, const warehouse& w);
  robot* searchRobot(const string& id) const;
  void displayAllRobots() const;
  void displayAvailableRobots() const;
  bool updateBattery(const string& id, double new_battery);
  bool assignRobot(const string& id, const string& task_id);
  bool releaseRobot(const string& id);
  bool setMaintenance(const string& id, bool make_unavailable);
};
