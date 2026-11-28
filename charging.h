#pragma once
#include <string>
#include "LinkedList.h"
#include "Queue.h"
#include "Robot.h"
#include "warehouse.h"

using namespace std;

const int MAX_CHARGER_SLOTS = 20;

class charging_station {
 public:
  string id;
  string location_id;
  int total_slots;
  string slots[MAX_CHARGER_SLOTS];
  queue<string> waiting_queue;

  charging_station() {
    total_slots = 0;
    for (int i = 0; i < MAX_CHARGER_SLOTS; i++) {
      slots[i] = "";
    }
  }
};

class charging_manager {
 public:
  linkedList<charging_station*> stations;

  void loadChargers(const warehouse& w);
  bool addStation(string id, string location_id, int total_slots, const warehouse& w);
  charging_station* searchStation(const string& id) const;
  void displayAllStations();
  void displayRobotsNeedingCharging(const fleet_manager& fleet) const;
  bool hasFreeSlot(const charging_station* st) const;
  charging_station* findBestStation(robot* r, warehouse& w, int& out_dist);
  bool sendRobotToCharge(const string& robot_id, warehouse& w, fleet_manager& fleet);
  bool completeCharging(const string& robot_id, fleet_manager& fleet);
  bool removeFromQueue(const string& robot_id, fleet_manager& fleet);
  bool updateRobotBattery(const string& robot_id, double new_battery, fleet_manager& fleet);
};
