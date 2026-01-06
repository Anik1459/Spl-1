

#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <algorithm>
#include <cmath>
#include <string>
#include<iomanip>


using namespace std;

// CONSTANTS 

const double INF = 1.0e9;
const int MAX_NODES = 50;
const int MAX_PACKAGES = 100;
const int MAX_EDGES = 500;
 

//  GLOBAL DATA ARRAYS 

// Graph Data
int graphNumNodes = 0;
string graphNodeNames[MAX_NODES];
vector<int> graphAdjList[MAX_NODES];
double graphDistMatrix[MAX_NODES][MAX_NODES];
double graphTimeMatrix[MAX_NODES][MAX_NODES];

// Edge Data
int numEdges = 0;
int edgeFrom[MAX_EDGES];
int edgeTo[MAX_EDGES];
double edgeDistance[MAX_EDGES];
 
// Package Data
int numPackages = 0;
int packageId[MAX_PACKAGES];
int packageDestination[MAX_PACKAGES];
double packageWeight[MAX_PACKAGES];

void initializeGraph(int numNodes) {
    graphNumNodes = numNodes;
    
    // Initialize distance matrix with infinity
    for (int i = 0; i < graphNumNodes; i++) {
        for (int j = 0; j < graphNumNodes; j++) {
            if (i == j) {
                graphDistMatrix[i][j] = 0;
                graphTimeMatrix[i][j] = 0;
            } else {
                graphDistMatrix[i][j] = INF;
                graphTimeMatrix[i][j] = INF;
            }
        }
    }
    
    // Clear adjacency lists
    for (int i = 0; i < MAX_NODES; i++) {
        graphAdjList[i].clear();
    }
    
    numEdges = 0;
}

void setNodeName(int nodeId, const string& name) {
    if (nodeId >= 0 && nodeId < graphNumNodes) {
        graphNodeNames[nodeId] = name;
    } 
}

string getNodeName(int nodeId) {
    if (nodeId >= 0 && nodeId < graphNumNodes) {
        return graphNodeNames[nodeId];
    }
    return "Unknown";
}

void addEdge(int from, int to, double distance, double baseTime = 0)
{
    if (numEdges >= MAX_EDGES) return;
    
    edgeFrom[numEdges] = from;
    edgeTo[numEdges] = to;
    edgeDistance[numEdges] = distance;
    numEdges++;
    
    // Update adjacency list
    graphAdjList[from].push_back(to);
    
    // Update distance matrix
    graphDistMatrix[from][to] = distance;
}
void addBidirectionalEdge(int node1, int node2, double distance, double baseTime = 0)
{
    addEdge(node1, node2, distance, baseTime );
    addEdge(node2, node1, distance, baseTime);
}

void addPackage(int id, int destination,  double weight,const string& customerName)
{
    if (numPackages >= MAX_PACKAGES) return;
    
    packageId[numPackages] = id;
    packageDestination[numPackages] = destination;

    packageWeight[numPackages] = weight;
    
    numPackages++;
}

void printPackageSummary() {
    cout << "\n================================================================================\n";
    cout << "                          PACKAGE INVENTORY                                     \n";
    cout << "================================================================================\n\n";
    
    cout << "  ID   Destination  Weight\n";
    cout << "  ---  -----------  ----------  \n";
    
    for (int i = 0; i < numPackages; i++) {
        cout << "  " << setw(3) << packageId[i] << "  "
             << setw(11) << getNodeName(packageDestination[i]) << "  "
             << setw(6) << fixed << setprecision(1) << packageWeight[i] << "kg " "\n";
    }

    cout << "\n  Total Packages: " << numPackages << "\n";
    cout << string(80, '=') << "\n";
}

void manualNetworkInput() {
    cout << "\n=== MANUAL NETWORK SETUP ===\n\n";
    
    int numNodes;
    cout << "Enter number of cities/nodes (max " << MAX_NODES << "): ";
    cin >> numNodes;
    cin.ignore();
    
    if (numNodes <= 0 || numNodes > MAX_NODES) {
        cout << "[ERROR] Invalid number of nodes. Using default.\n";
        numNodes = 5;
    }
    
    initializeGraph(numNodes);
    
    cout << "\nEnter names for each city:\n";
    for (int i = 0; i < numNodes; i++) {
        cout << "  City " << i << ": ";
        string name;
        getline(cin, name);
        string finalName;

        if (name.empty()) {
            finalName = "City_" + to_string(i);
        } else {
            finalName = name;
        }

        setNodeName(i, finalName);
            }
    
    int numEdges;
    cout << "\nEnter number of road connections/edges: ";
    cin >> numEdges;
    
    cout << "\n=== ROAD CONNECTION INPUT ===\n";
    cout << "Note: Enter bidirectional roads (connects both ways)\n";
    cout << "Format: from_city to_city distance(km) \n\n";
    
    for (int i = 0; i < numEdges; i++) {
        int from, to;
        double distance;
        
        cout << "  Road " << (i + 1) << ":\n";
        cout << "    From city [index]: ";
        cin >> from;
        cout << "    To city [index]: ";
        cin >> to;
        cout << "    Distance (km): ";
        cin >> distance;
        // Validate input
        if (from >= 0 && from < graphNumNodes && to >= 0 && to < graphNumNodes && 
            distance > 0) {
            
            addBidirectionalEdge(from, to, distance, 0);
            cout << "    [OK] Road added: " << getNodeName(from) << " <-> " 
                 << getNodeName(to) << " (" << distance << " km", " \n";
        } 
        else {
            cout << "    [ERROR] Invalid input. Road skipped.\n";
            
        }
        cout << "\n";
    }
    
    cout << "[SYSTEM] Network setup complete with " << numEdges << " road connections!\n";
    
}

void manualPackageInput() {
    cout << "\n=== MANUAL PACKAGE INPUT ===\n\n";
    
    int numPkgs;
    cout << "Enter number of packages: ";
    cin >> numPkgs;
    cin.ignore();
    
    for (int i = 0; i < numPkgs; i++) {
        cout << "--- Package " << (i + 1) << " ---\n";
        
        int destination, fragile;
        double weight;
        string customerName, deliveryWindow;
        
        cout << "  Destination city [index]: ";
        cin >> destination;
        cin.ignore();
        
        cout << "  Customer name: ";
        getline(cin, customerName);
        
        cout << "  Weight (kg): ";
        cin >> weight;
        
        cin.ignore();
        
        
        if (destination >= 0 && destination < graphNumNodes &&  weight > 0) {
            
            addPackage(i + 1, destination, weight, customerName);
            
            cout << "  [OK] Package added for delivery to " << getNodeName(destination) << "\n";
        } 
        else {
            cout << "  [ERROR] Invalid package data. Skipping.\n";
        }
        cout << "\n";
    }
    
    printPackageSummary();
}

int main(){
    cout << "\n" << string(80, '=') << "\n\n";
    cout << "              SMARTCOURIER: ADAPTIVE DELIVERY OPTIMIZER \n";
    cout << "                     \n";
    cout << "         Priority-Based Package Delivery with Real-Time Traffic\n\n";
    cout << string(80, '=') << "\n\n";

    cout << string(80, '=') << "\n";
    cout << "                         SYSTEM SETUP MODE\n";
    cout << string(80, '=') << "\n\n";
    
    cout << "Choose setup mode:\n";
    cout << "  [1] Manual Input - Enter your own cities, distances, and packages\n";
    cout << "Enter your choice (1): ";
    
    int choice;
    cin >> choice;
    cin.ignore();
    
    if (choice == 1) {
        cout << "[MODE] Manual Input Selected\n";
        cout << string(80, '=') << "\n";
        manualNetworkInput();
        manualPackageInput();
    } 

}


