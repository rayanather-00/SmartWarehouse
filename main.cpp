#include <iostream>

#include "charging.h"
#include "robot.h"
#include "warehouse.h"

using namespace std;

int main() {
  warehouse w;
  fleet_manager fleet;
  charging_manager chargers;

  cout << "========== 1. LOADING ALL DATA ==========\n";
  w.LoadLocations();
  fleet.loadRobots(w);
  chargers.loadChargers(w);

  cout << "\n--- WAREHOUSE (MODULE 1) ---\n";
  w.displayAll();
  cout << "BFS L01 -> L14: " << (w.checkIfConnectedBFS("L01", "L14") ? "Connected" : "Not Connected") << endl;
  w.displayRoute("L01", "L14");
  cout << "Blocking L09-L14...\n";
  w.blockPathway("L09", "L14");
  w.displayRoute("L01", "L14");
  cout << "Reopening L09-L14...\n";
  w.reopenPathway("L09", "L14");

  cout << "\n--- ROBOT FLEET (MODULE 4) ---\n";
  fleet.displayAllRobots();
  fleet.registerRobot("RB99", "Picker", "L01", 90.0, 25, "Available", "NONE", w);  // expect reject
  fleet.registerRobot("RB06", "Picker", "L13", 15.0, 10, "Available", "NONE", w);  // valid 6th robot
  fleet.assignRobot("RB01", "O101");
  fleet.assignRobot("RB01", "O102");  // expect reject (already assigned)
  fleet.releaseRobot("RB01");

  cout << "\n--- CHARGING STATIONS (MODULE 7) ---\n";
  chargers.displayAllStations();

  // drop batteries below 20% so we can test charging & waiting queue
  fleet.updateBattery("RB01", 15.0);
  fleet.updateBattery("RB02", 15.0);
  fleet.updateBattery("RB03", 15.0);
  fleet.updateBattery("RB04", 15.0);
  fleet.updateBattery("RB05", 15.0);

  cout << "\n--- ROBOTS NEEDING CHARGING (< 20%) ---\n";
  chargers.displayRobotsNeedingCharging(fleet);

  cout << "\n--- FILLING C01 (2 SLOTS) & C02 (3 SLOTS) + QUEUE TEST ---\n";
  chargers.sendRobotToCharge("RB05", w, fleet);  // goes to C01 (nearest free)
  chargers.sendRobotToCharge("RB04", w, fleet);  // goes to C01 (fills C01)
  chargers.sendRobotToCharge("RB03", w, fleet);  // C01 full -> goes to C02
  chargers.sendRobotToCharge("RB02", w, fleet);  // goes to C02
  chargers.sendRobotToCharge("RB01", w, fleet);  // fills C02
  chargers.sendRobotToCharge("RB06", w, fleet);  // all full -> joins C01 waiting queue

  cout << "\n--- STATIONS AFTER FILLING ---\n";
  chargers.displayAllStations();

  cout << "\n--- COMPLETE CHARGING FOR RB05 (RB06 SHOULD AUTO-ENTER SLOT) ---\n";
  chargers.completeCharging("RB05", fleet);
  chargers.displayAllStations();

  cout << "\n--- UNREACHABLE / DEAD BATTERY TEST ---\n";
  fleet.registerRobot("RB07", "Heavy-duty", "L01", 0.5, 40, "Available", "NONE", w);
  chargers.sendRobotToCharge("RB07", w, fleet);  // expect Unavailable

  cout << "\n--- FINAL ROBOT STATUSES ---\n";
  fleet.displayAllRobots();

  return 0;
}
