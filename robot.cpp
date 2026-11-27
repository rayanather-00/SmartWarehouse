#include "Robot.h"

#include <iomanip>
#include <iostream>

#include "file_handler.h"

using namespace std;


static string trim_robot_str(const string& s) {
  int start = 0;
  int end = (int)s.length() - 1;
  while (start <= end && (s[start] == ' ' || s[start] == '\t' || s[start] == '\r')) {
    start++;
  }
  while (end >= start && (s[end] == ' ' || s[end] == '\t' || s[end] == '\r')) {
    end--;
  }
  if (start > end) {
    return "";
  }
  return s.substr(start, end - start + 1);
}

static bool is_valid_robot_status(const string& status) {
  return (status == "Available" ||
          status == "Assigned" ||
          status == "Waiting for Charging" ||
          status == "Charging" ||
          status == "Unavailable");
}

static robot* create_robot_by_type(const string& type) {
  if (type == "Picker") {
    return new picker();
  }
  if (type == "Carrier") {
    return new carrier();
  }
  if (type == "Heavy-duty") {
    return new heavy_duty();
  }
  return nullptr;
}


void fleet_manager::loadRobots(const warehouse& w) {
  warehouse_data data;
  if (!load_robots(data)) {
    cout << "Error: could not open robots.txt\n";
    return;
  }

  for (int i = 0; i < data.robot_count; i++) {
    string id = trim_robot_str(data.robots[i].robot_id);
    string type = trim_robot_str(data.robots[i].type);
    string location = trim_robot_str(data.robots[i].current_location);
    double battery = data.robots[i].battery;
    int capacity = data.robots[i].max_capacity;
    string status = trim_robot_str(data.robots[i].status);
    string task = trim_robot_str(data.robots[i].assigned_task);

    registerRobot(id, type, location, battery, capacity, status, task, w);
  }
}

bool fleet_manager::registerRobot(string id, string type, string location, double battery, int max_capacity, string status, string task, const warehouse& w) {
  if (id.empty()) {
    cout << "Invalid robot ID.\n";
    return false;
  }
  if (searchRobot(id) != nullptr) {
    cout << "Duplicate robot ID: " << id << endl;
    return false;
  }
  if (w.searchLocation(location) == nullptr) {
    cout << "Rejected robot " << id << ": location " << location << " does not exist.\n";
    return false;
  }
  if (battery < 0.0 || battery > 100.0) {
    cout << "Rejected robot " << id << ": battery must be between 0 and 100.\n";
    return false;
  }
  if (!is_valid_robot_status(status)) {
    cout << "Rejected robot " << id << ": invalid status " << status << endl;
    return false;
  }

  robot* r = create_robot_by_type(type);
  if (r == nullptr) {
    cout << "Rejected robot " << id << ": unknown type " << type << endl;
    return false;
  }

  if (task == "NONE" || task == "none") {
    task = "";
  }
  if (status == "Available") {
    task = "";
  }

  r->id = id;
  r->type = type;
  r->current_location = location;
  r->battery = battery;
  r->max_capacity = max_capacity;
  r->status = status;
  r->assigned_task = task;

  if (!r->is_valid_capacity()) {
    cout << "Rejected robot " << id << ": capacity " << max_capacity << " violates " << type << " limit.\n";
    delete r;
    return false;
  }

  robots.insert_back(r);
  return true;
}

robot* fleet_manager::searchRobot(const string& id) const {
  list_node<robot*>* curr = robots.head;
  while (curr != nullptr) {
    if (curr->data->id == id) {
      return curr->data;
    }
    curr = curr->next;
  }
  return nullptr;
}

void fleet_manager::displayAllRobots() const {
  if (robots.is_empty()) {
    cout << "No robots in fleet.\n";
    return;
  }

  list_node<robot*>* curr = robots.head;
  while (curr != nullptr) {
    robot* r = curr->data;
    cout << r->id << " | Type: " << r->type
         << " | Loc: " << r->current_location
         << " | Battery: " << fixed << setprecision(1) << r->battery << "%"
         << " | Cap: " << r->max_capacity << " kg"
         << " | Status: " << r->status
         << " | Task: " << (r->assigned_task.empty() ? "NONE" : r->assigned_task);
    if (r->battery < 20.0) {
      cout << " [NEEDS CHARGING]";
    }
    cout << endl;
    curr = curr->next;
  }
}

void fleet_manager::displayAvailableRobots() const {
  bool any = false;
  list_node<robot*>* curr = robots.head;
  while (curr != nullptr) {
    robot* r = curr->data;
    if (r->status == "Available") {
      any = true;
      cout << r->id << " | Type: " << r->type
           << " | Loc: " << r->current_location
           << " | Battery: " << fixed << setprecision(1) << r->battery << "%"
           << " | Cap: " << r->max_capacity << " kg";
      if (r->battery < 20.0) {
        cout << " [NEEDS CHARGING]";
      }
      cout << endl;
    }
    curr = curr->next;
  }
  if (!any) {
    cout << "No available robots.\n";
  }
}

bool fleet_manager::updateBattery(const string& id, double new_battery) {
  robot* r = searchRobot(id);
  if (r == nullptr) {
    cout << "Robot " << id << " not found.\n";
    return false;
  }
  if (new_battery < 0.0 || new_battery > 100.0) {
    cout << "Battery level must be between 0 and 100.\n";
    return false;
  }
  r->battery = new_battery;
  return true;
}

bool fleet_manager::assignRobot(const string& id, const string& task_id) {
  robot* r = searchRobot(id);
  if (r == nullptr) {
    cout << "Robot " << id << " not found.\n";
    return false;
  }
  if (task_id.empty()) {
    cout << "Task ID cannot be empty.\n";
    return false;
  }
  if (r->status != "Available" || !r->assigned_task.empty()) {
    cout << "Robot " << id << " is not available for assignment (Status: " << r->status << ").\n";
    return false;
  }
  r->status = "Assigned";
  r->assigned_task = task_id;
  return true;
}

bool fleet_manager::releaseRobot(const string& id) {
  robot* r = searchRobot(id);
  if (r == nullptr) {
    cout << "Robot " << id << " not found.\n";
    return false;
  }
  r->assigned_task = "";
  r->status = "Available";
  if (r->battery < 20.0) {
    cout << "Notice: Robot " << id << " released with low battery ("
         << fixed << setprecision(1) << r->battery << "%). Needs charging!\n";
  }
  return true;
}

bool fleet_manager::setMaintenance(const string& id, bool make_unavailable) {
  robot* r = searchRobot(id);
  if (r == nullptr) {
    cout << "Robot " << id << " not found.\n";
    return false;
  }

  if (make_unavailable) {
    if (!r->assigned_task.empty() || r->status == "Assigned") {
      cout << "Cannot mark " << id << " unavailable: robot has an active task.\n";
      return false;
    }
    if (r->status == "Charging") {
      cout << "Cannot mark " << id << " unavailable: robot is currently charging.\n";
      return false;
    }
    if (r->status == "Waiting for Charging") {
      cout << "Cannot mark " << id << " unavailable: remove it from the charging queue first.\n";
      return false;
    }
    if (r->status == "Unavailable") {
      cout << "Robot " << id << " is already unavailable.\n";
      return false;
    }
    r->status = "Unavailable";
    return true;
  }
  else {
    if (r->status != "Unavailable") {
      cout << "Robot " << id << " is not currently unavailable for maintenance.\n";
      return false;
    }
    r->status = "Available";
    return true;
  }
}
