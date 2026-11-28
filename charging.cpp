#include "Charging.h"
#include <iomanip>
#include <iostream>
#include "file_handler.h"
#include "linkedList.h"
using namespace std;

static string trim_charger_str(const string& s) {
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

void charging_manager::loadChargers(const warehouse& w) {
  warehouse_data data;
  if (!load_chargers(data)) {
    cout << "Error: could not open chargers.txt\n";
    return;
  }

  for (int i = 0; i < data.charger_count; i++) {
    string id = trim_charger_str(data.chargers[i].charging_station_id);
    string loc = trim_charger_str(data.chargers[i].location_id);
    int slots = data.chargers[i].total_slots;
    addStation(id, loc, slots, w);
  }
}

bool charging_manager::addStation(string id, string location_id, int total_slots, const warehouse& w) {
  if (id.empty() || searchStation(id) != nullptr) {
    cout << "Invalid or duplicate charging station ID.\n";
    return false;
  }
  // station must sit on a CHARGING type location
  location* loc = w.searchLocation(location_id);
  if (loc == nullptr || loc->type != "CHARGING") {
    cout << "Rejected station " << id << ": " << location_id << " is not a CHARGING location.\n";
    return false;
  }
  if (total_slots <= 0 || total_slots > MAX_CHARGER_SLOTS) {
    cout << "Rejected station " << id << ": invalid slot count.\n";
    return false;
  }

  charging_station* st = new charging_station();
  st->id = id;
  st->location_id = location_id;
  st->total_slots = total_slots;
  stations.insert_back(st);
  return true;
}

charging_station* charging_manager::searchStation(const string& id) const {
  list_node<charging_station*>* curr = stations.head;
  while (curr != nullptr) {
    if (curr->data->id == id) {
      return curr->data;
    }
    curr = curr->next;
  }
  return nullptr;
}

bool charging_manager::hasFreeSlot(const charging_station* st) const {
  if (st == nullptr) {
    return false;
  }
  for (int i = 0; i < st->total_slots; i++) {
    if (st->slots[i].empty()) {
      return true;
    }
  }
  return false;
}

void charging_manager::displayAllStations() {
  if (stations.is_empty()) {
    cout << "No charging stations loaded.\n";
    return;
  }

  list_node<charging_station*>* curr = stations.head;
  while (curr != nullptr) {
    charging_station* st = curr->data;
    int free_count = 0;
    for (int i = 0; i < st->total_slots; i++) {
      if (st->slots[i].empty()) {
        free_count++;
      }
    }

    cout << st->id << " (Location: " << st->location_id << ") | Slots: "
         << free_count << "/" << st->total_slots << " free\n";

    cout << "  Currently charging: ";
    bool any_charging = false;
    for (int i = 0; i < st->total_slots; i++) {
      if (!st->slots[i].empty()) {
        cout << st->slots[i] << " ";
        any_charging = true;
      }
    }
    if (!any_charging) {
      cout << "None";
    }
    cout << endl;

    // print queue without removing anyone
    cout << "  Waiting queue: Front -> ";
    queue_node<string>* qcurr = st->waiting_queue.front;
    while (qcurr != nullptr) {
      cout << qcurr->data << " -> ";
      qcurr = qcurr->next;
    }
    cout << "NULL\n";

    curr = curr->next;
  }
}

void charging_manager::displayRobotsNeedingCharging(const fleet_manager& fleet) const {
  bool any = false;
  list_node<robot*>* curr = fleet.robots.head;
  while (curr != nullptr) {
    robot* r = curr->data;
    if (r->battery < 20.0) {
      any = true;
      cout << r->id << " | Loc: " << r->current_location
           << " | Battery: " << fixed << setprecision(1) << r->battery << "%"
           << " | Status: " << r->status << endl;
    }
    curr = curr->next;
  }
  if (!any) {
    cout << "No robots currently need charging (< 20.0%).\n";
  }
}

charging_station* charging_manager::findBestStation(robot* r, warehouse& w, int& out_dist) {
  out_dist = -1;
  if (r == nullptr) {
    return nullptr;
  }

  charging_station* best_free = nullptr;
  int best_free_dist = -1;
  charging_station* best_full = nullptr;
  int best_full_dist = -1;

  list_node<charging_station*>* curr = stations.head;
  while (curr != nullptr) {
    charging_station* st = curr->data;
    linkedList<string> temp_route;
    int dist = w.findShortestRoute(r->current_location, st->location_id, temp_route);
    temp_route.clear();

    if (dist != -1) {
      // empty leg battery cost to reach station
      double travel_cost = dist * r->get_battery_rate();
      if (r->battery >= travel_cost) {
        if (hasFreeSlot(st)) {
          if (best_free == nullptr || dist < best_free_dist || (dist == best_free_dist && st->id < best_free->id)) {
            best_free = st;
            best_free_dist = dist;
          }
        }
        else {
          if (best_full == nullptr || dist < best_full_dist || (dist == best_full_dist && st->id < best_full->id)) {
            best_full = st;
            best_full_dist = dist;
          }
        }
      }
    }
    curr = curr->next;
  }

  // prefer station with free slot, otherwise nearest reachable full station
  if (best_free != nullptr) {
    out_dist = best_free_dist;
    return best_free;
  }
  if (best_full != nullptr) {
    out_dist = best_full_dist;
    return best_full;
  }
  return nullptr;
}

bool charging_manager::sendRobotToCharge(const string& robot_id, warehouse& w, fleet_manager& fleet) {
  robot* r = fleet.searchRobot(robot_id);
  if (r == nullptr) {
    cout << "Robot " << robot_id << " not found.\n";
    return false;
  }
  if (r->status != "Available") {
    cout << "Robot " << robot_id << " cannot be sent to charge (Status: " << r->status << ").\n";
    return false;
  }
  if (r->battery >= 100.0) {
    cout << "Robot " << robot_id << " is already at 100.0% battery.\n";
    return false;
  }

  int dist = -1;
  charging_station* st = findBestStation(r, w, dist);
  if (st == nullptr) {
    // not enough battery or all paths blocked, needs manual help
    r->status = "Unavailable";
    cout << "Robot " << robot_id << " cannot reach any charging station. Status set to Unavailable.\n";
    return false;
  }

  // deduct battery used to travel to station and update location
  double travel_cost = dist * r->get_battery_rate();
  r->battery -= travel_cost;
  if (r->battery < 0.0) {
    r->battery = 0.0;
  }
  r->current_location = st->location_id;

  // put in free slot if available
  for (int i = 0; i < st->total_slots; i++) {
    if (st->slots[i].empty()) {
      st->slots[i] = r->id;
      r->status = "Charging";
      cout << "Robot " << r->id << " started charging at " << st->id
           << " (" << st->location_id << "), distance " << dist << ".\n";
      return true;
    }
  }

  // otherwise join this station's waiting queue
  st->waiting_queue.enqueue(r->id);
  r->status = "Waiting for Charging";
  cout << "All reachable stations full. Robot " << r->id << " joined waiting queue at " << st->id << ".\n";
  return true;
}

bool charging_manager::completeCharging(const string& robot_id, fleet_manager& fleet) {
  list_node<charging_station*>* curr = stations.head;
  while (curr != nullptr) {
    charging_station* st = curr->data;
    for (int i = 0; i < st->total_slots; i++) {
      if (st->slots[i] == robot_id) {
        st->slots[i] = "";
        robot* r = fleet.searchRobot(robot_id);
        if (r != nullptr) {
          r->battery = 100.0;
          r->status = "Available";
        }
        cout << "Robot " << robot_id << " finished charging (100.0%) at " << st->id << ".\n";

        // bring next waiting robot in (FIFO)
        if (!st->waiting_queue.is_empty()) {
          string next_id;
          st->waiting_queue.dequeue(next_id);
          st->slots[i] = next_id;
          robot* next_r = fleet.searchRobot(next_id);
          if (next_r != nullptr) {
            next_r->status = "Charging";
            next_r->current_location = st->location_id;
          }
          cout << "Robot " << next_id << " moved from queue into slot at " << st->id << ".\n";
        }
        return true;
      }
    }
    curr = curr->next;
  }

  cout << "Robot " << robot_id << " is not currently charging at any station.\n";
  return false;
}

bool charging_manager::removeFromQueue(const string& robot_id, fleet_manager& fleet) {
  list_node<charging_station*>* curr = stations.head;
  while (curr != nullptr) {
    charging_station* st = curr->data;
    if (st->waiting_queue.remove(robot_id)) {
      robot* r = fleet.searchRobot(robot_id);
      if (r != nullptr) {
        r->status = "Available";
      }
      cout << "Removed " << robot_id << " from waiting queue at " << st->id << ".\n";
      return true;
    }
    curr = curr->next;
  }

  cout << "Robot " << robot_id << " was not found in any charging queue.\n";
  return false;
}

bool charging_manager::updateRobotBattery(const string& robot_id, double new_battery, fleet_manager& fleet) {
  return fleet.updateBattery(robot_id, new_battery);
}
