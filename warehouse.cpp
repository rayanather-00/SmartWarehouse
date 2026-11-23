#include "warehouse.h"

#include <climits>
#include <iostream>
#include <string>

#include "Queue.h"
#include "Stack.h"
#include "file_handler.h"

using namespace std;
// these are helpers
static string trim_str(const string& s) {
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

static pathway* findPathway(location* loc, const string& dest_id) {
  if (loc == nullptr) {
    return nullptr;
  }
  list_node<pathway>* current = loc->pathways.head;
  while (current != nullptr) {
    if (current->data.id == dest_id) {
      return &current->data;
    }
    current = current->next;
  }
  return nullptr;
}

static int getIndex(location** nodes, int n, const string& id) {
  for (int i = 0; i < n; i++) {
    if (nodes[i]->id == id) {
      return i;
    }
  }
  return -1;
}

// the main funcs
void warehouse::LoadLocations() {
  warehouse_data data;
  if (!load_locations(data)) {
    cout << "Error: could not open locations.txt\n";
    return;
  }
  for (int i = 0; i < data.location_count; i++) {
    string id = trim_str(data.locations[i].location_id);
    string name = trim_str(data.locations[i].name);
    string type = trim_str(data.locations[i].type);
    addLocation(id, name, type);
  }

  if (!load_pathways(data)) {
    cout << "Error: could not open pathways.txt\n";
    return;
  }
  for (int i = 0; i < data.pathway_count; i++) {
    string loc1 = trim_str(data.pathways[i].location1);
    string loc2 = trim_str(data.pathways[i].location2);
    int dist = data.pathways[i].distance;
    string status = trim_str(data.pathways[i].status);
    addPathway(loc1, loc2, dist, status);
  }
}

void warehouse::displayAll() {
  if (locations.is_empty()) {
    cout << "No locations in warehouse.\n";
    return;
  }
  list_node<location*>* curr = locations.head;
  while (curr != nullptr) {
    location* loc = curr->data;
    cout << loc->id << " (" << loc->type << ") - " << loc->name << endl;
    if (loc->pathways.is_empty()) {
      cout << "  No pathways\n";
    }
    else {
      list_node<pathway>* edge = loc->pathways.head;
      while (edge != nullptr) {
        cout << "  -> " << edge->data.id << " (Distance: " << edge->data.distance << ")";
        if (edge->data.status == "BLOCKED") {
          cout << " [BLOCKED]";
        }
        else {
          cout << " [OPEN]";
        }
        cout << endl;
        edge = edge->next;
      }
    }
    curr = curr->next;
  }
}

location* warehouse::searchLocation(const string& id) const {
  list_node<location*>* current = locations.head;
  while (current != nullptr) {
    if (current->data->id == id) {
      return current->data;
    }
    current = current->next;
  }
  return nullptr;
}

void warehouse::addLocation(string id, string name, string type) {
  if (id.empty() || name.empty()) {
    cout << "Invalid location ID or name.\n";
    return;
  }
  if (type != "RACK" && type != "PACKING" && type != "CHARGING") {
    cout << "Invalid location type: " << type << endl;
    return;
  }
  if (searchLocation(id) != nullptr) {
    cout << "Duplicate location ID: " << id << endl;
    return;
  }
  location* temp = new location(id, name, type);
  locations.insert_back(temp);
}

void warehouse::addPathway(string location1, string location2, int distance, string status) {
  if (location1 == location2) {
    cout << "Cannot add pathway from a location to itself.\n";
    return;
  }
  if (distance <= 0) {
    cout << "Pathway distance must be greater than 0.\n";
    return;
  }
  if (status != "OPEN" && status != "BLOCKED") {
    cout << "Invalid pathway status: " << status << endl;
    return;
  }
  location* u = searchLocation(location1);
  location* v = searchLocation(location2);
  if (u == nullptr || v == nullptr) {
    cout << "One or both locations do not exist.\n";
    return;
  }
  if (findPathway(u, location2) != nullptr) {
    cout << "Duplicate pathway between " << location1 << " and " << location2 << ".\n";
    return;
  }

  pathway p1;
  p1.id = location2;
  p1.distance = distance;
  p1.status = status;
  u->pathways.insert_back(p1);

  pathway p2;
  p2.id = location1;
  p2.distance = distance;
  p2.status = status;
  v->pathways.insert_back(p2);
}

void warehouse::removePathway(string location1, string location2) {
  location* u = searchLocation(location1);
  location* v = searchLocation(location2);
  if (u == nullptr || v == nullptr) {
    cout << "One or both locations do not exist.\n";
    return;
  }

  bool removed = false;
  list_node<pathway>* prev = nullptr;
  list_node<pathway>* curr = u->pathways.head;
  while (curr != nullptr) {
    if (curr->data.id == location2) {
      u->pathways.remove_node(prev, curr);
      removed = true;
      break;
    }
    prev = curr;
    curr = curr->next;
  }

  if (!removed) {
    cout << "Pathway between " << location1 << " and " << location2 << " does not exist.\n";
    return;
  }

  prev = nullptr;
  curr = v->pathways.head;
  while (curr != nullptr) {
    if (curr->data.id == location1) {
      v->pathways.remove_node(prev, curr);
      break;
    }
    prev = curr;
    curr = curr->next;
  }
}

void warehouse::blockPathway(string location1, string location2) {
  location* u = searchLocation(location1);
  location* v = searchLocation(location2);
  if (u == nullptr || v == nullptr) {
    cout << "One or both locations do not exist.\n";
    return;
  }
  pathway* p1 = findPathway(u, location2);
  pathway* p2 = findPathway(v, location1);
  if (p1 == nullptr || p2 == nullptr) {
    cout << "Pathway does not exist.\n";
    return;
  }
  if (p1->status == "BLOCKED") {
    cout << "Pathway is already blocked.\n";
    return;
  }
  p1->status = "BLOCKED";
  p2->status = "BLOCKED";
}

void warehouse::reopenPathway(string location1, string location2) {
  location* u = searchLocation(location1);
  location* v = searchLocation(location2);
  if (u == nullptr || v == nullptr) {
    cout << "One or both locations do not exist.\n";
    return;
  }
  pathway* p1 = findPathway(u, location2);
  pathway* p2 = findPathway(v, location1);
  if (p1 == nullptr || p2 == nullptr) {
    cout << "Pathway does not exist.\n";
    return;
  }
  if (p1->status == "OPEN") {
    cout << "Pathway is already open.\n";
    return;
  }
  p1->status = "OPEN";
  p2->status = "OPEN";
}

bool warehouse::checkIfConnectedBFS(string location1, string location2) {
  if (searchLocation(location1) == nullptr || searchLocation(location2) == nullptr) {
    cout << "Error: start or end location does not exist.\n";
    return false;
  }
  if (location1 == location2) {
    return true;
  }

  int n = locations.get_size();
  location** nodes = new location*[n];
  bool* visited = new bool[n];

  list_node<location*>* curr = locations.head;
  for (int i = 0; i < n; i++) {
    nodes[i] = curr->data;
    visited[i] = false;
    curr = curr->next;
  }

  int start_idx = getIndex(nodes, n, location1);
  int end_idx = getIndex(nodes, n, location2);

  queue<int> q;
  visited[start_idx] = true;
  q.enqueue(start_idx);

  bool found = false;
  while (!q.is_empty()) {
    int u = -1;
    q.dequeue(u);

    list_node<pathway>* edge = nodes[u]->pathways.head;
    while (edge != nullptr) {
      if (edge->data.status != "BLOCKED") {
        int v = getIndex(nodes, n, edge->data.id);
        if (v != -1 && !visited[v]) {
          if (v == end_idx) {
            found = true;
            break;
          }
          visited[v] = true;
          q.enqueue(v);
        }
      }
      edge = edge->next;
    }
    if (found) {
      break;
    }
  }

  while (!q.is_empty()) {
    int dummy = 0;
    q.dequeue(dummy);
  }
  delete[] nodes;
  delete[] visited;
  return found;
}

int warehouse::findShortestRoute(string location1, string location2, linkedList<string>& route_out) {
  route_out.clear();

  if (searchLocation(location1) == nullptr || searchLocation(location2) == nullptr) {
    cout << "Error: start or end location does not exist.\n";
    return -1;
  }
  if (location1 == location2) {
    route_out.insert_back(location1);
    return 0;
  }

  int n = locations.get_size();
  location** nodes = new location*[n];
  int* dist = new int[n];
  int* hops = new int[n];
  int* parent = new int[n];
  bool* visited = new bool[n];

  list_node<location*>* curr = locations.head;
  for (int i = 0; i < n; i++) {
    nodes[i] = curr->data;
    dist[i] = INT_MAX;
    hops[i] = INT_MAX;
    parent[i] = -1;
    visited[i] = false;
    curr = curr->next;
  }

  int start_idx = getIndex(nodes, n, location1);
  int end_idx = getIndex(nodes, n, location2);

  dist[start_idx] = 0;
  hops[start_idx] = 0;

  for (int count = 0; count < n; count++) {
    int u = -1;
    for (int i = 0; i < n; i++) {
      if (!visited[i] && dist[i] < INT_MAX) {
        if (u == -1 ||
            dist[i] < dist[u] ||
            (dist[i] == dist[u] && hops[i] < hops[u]) ||
            (dist[i] == dist[u] && hops[i] == hops[u] && nodes[i]->id < nodes[u]->id)) {
          u = i;
        }
      }
    }

    if (u == -1 || u == end_idx) {
      break;
    }
    visited[u] = true;

    list_node<pathway>* edge = nodes[u]->pathways.head;
    while (edge != nullptr) {
      if (edge->data.status != "BLOCKED") {
        int v = getIndex(nodes, n, edge->data.id);
        if (v != -1 && !visited[v]) {
          int new_dist = dist[u] + edge->data.distance;
          int new_hops = hops[u] + 1;

          // Tie-break order: 1) distance, 2) fewer locations (hops), 3) smaller parent location ID
          bool better = false;
          if (new_dist < dist[v]) {
            better = true;
          }
          else if (new_dist == dist[v] && new_hops < hops[v]) {
            better = true;
          }
          else if (new_dist == dist[v] && new_hops == hops[v]) {
            if (parent[v] == -1 || nodes[u]->id < nodes[parent[v]]->id) {
              better = true;
            }
          }

          if (better) {
            dist[v] = new_dist;
            hops[v] = new_hops;
            parent[v] = u;
          }
        }
      }
      edge = edge->next;
    }
  }

  int total_distance = -1;
  if (dist[end_idx] != INT_MAX) {
    total_distance = dist[end_idx];
    stack<string> reverse_path;
    int trace = end_idx;
    while (trace != -1) {
      reverse_path.push(nodes[trace]->id);
      trace = parent[trace];
    }
    while (!reverse_path.is_empty()) {
      string loc_id;
      reverse_path.pop(loc_id);
      route_out.insert_back(loc_id);
    }
  }

  delete[] nodes;
  delete[] dist;
  delete[] hops;
  delete[] parent;
  delete[] visited;
  return total_distance;
}

void warehouse::displayRoute(string location1, string location2) {
  linkedList<string> route;
  int total_distance = findShortestRoute(location1, location2, route);
  if (total_distance == -1) {
    cout << "No route exists between " << location1 << " and " << location2 << ".\n";
    route.clear();
    return;
  }

  list_node<string>* curr = route.head;
  while (curr != nullptr) {
    cout << curr->data;
    if (curr->next != nullptr) {
      cout << " -> ";
    }
    curr = curr->next;
  }
  cout << ", total distance " << total_distance << endl;
  route.clear();
}
