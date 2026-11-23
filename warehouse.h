#include <iostream>
#include <string>

#include "linkedList.h"

using namespace std;

// edges
struct pathway {
  string id;  // point to id of destincation vertex/location
  int distance;
  string status;  // open or blocked

  pathway() {}
  pathway(string id, int distance, string status)
      : id(id), distance(distance), status(status) {}
};

// vertex
struct location {
  string id;
  string name;
  string type;  // rack, packing or charging
  linkedList<pathway> pathways;

  location() {}
  location(string id, string name, string type)
      : id(id), name(name), type(type) {}
};

class warehouse {
  linkedList<location*> locations;

 public:
  // all fucntions are void for now. change as needed ig.
  //  idk what needed depends on how they are called
  void LoadLocations();
  void displayAll();
  void addLocation(string id, string name, string type);
  void addPathway(string location1, string location2, int distance, string status = "OPEN");
  void removePathway(string location1, string location2);
  void blockPathway(string location1, string location2);
  void reopenPathway(string location1, string location2);
  location* searchLocation(const string& id) const;
  bool checkIfConnectedBFS(string location1, string location2);
  int findShortestRoute(string location1, string location2, linkedList<string>& route_out);
  void displayRoute(string location1, string location2);
};
