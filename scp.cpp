

#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <algorithm>
#include <cmath>
#include <string>



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


void initializeGraph(int numNodes) {
    graphNumNodes = numNodes;
    
    // Initialize distance matrix with infinity
    for (int i = 0; i < MAX_NODES; i++) {
        for (int j = 0; j < MAX_NODES; j++) {
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

void addEdge(int from, int to, double distance, double baseTime = 0, 
             double trafficFact = 1.0, double reliability = 0.9) {
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