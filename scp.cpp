
#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <algorithm>
#include <string>
#include<iomanip>

using namespace std;

// CONSTANTS 

const double INF = 1.0e9;
const int MAX_NODES = 50;
const int MAX_PACKAGES = 100;
const int MAX_EDGES = 500;
const int K_SHORTEST_PATHS = 3;


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

int depotNode = 0;

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

//===== FLOYED-WARSHALL ALGORITHM =====
void floydWarshallAllPairs(double allPairsDistances[MAX_NODES][MAX_NODES], 
                            int allPairsNext[MAX_NODES][MAX_NODES]){

    for(int i=0;i<graphNumNodes;i++){
        for(int j=0;j<graphNumNodes;j++){
           double currentDistanceValue = graphDistMatrix[i][j];
            allPairsDistances[i][j] = currentDistanceValue;
            if(i != j){
                if(graphDistMatrix[i][j] != INF){
                    allPairsNext[i][j] = j;
                }
                else{
                    allPairsNext[i][j] = -1;
                }
             }
             else{
                allPairsNext[i][j] = -1;
             }
        }
    } 

     // Floyd-Warshall
    for (int k = 0; k < graphNumNodes; k++) {
        for (int i = 0; i < graphNumNodes; i++) {
            for (int j = 0; j < graphNumNodes; j++) {
                if (allPairsDistances[i][k] != INF && allPairsDistances[k][j] != INF) {
                    double newDist = allPairsDistances[i][k] + allPairsDistances[k][j];
                    if (newDist < allPairsDistances[i][j]) {
                        allPairsDistances[i][j] = newDist;
                        allPairsNext[i][j] = allPairsNext[i][k];
                    }
                }
            }
        }
    }

    for (int i = 0; i < graphNumNodes; i++) {
            for (int j = 0; j < graphNumNodes; j++) {
                cout<<allPairsDistances[i][j]<<" ";
            }
            cout<<endl;
        }

}
//=====TSP solve using DP Mask
void tspsolve(int startNode,vector<int>& destinations,vector<vector<double>>& dpTable,
                vector<vector<int>>& parent ,vector<int>& optimalPath,double& bestCost){
    int n=destinations.size();
    if(n==0){
        optimalPath.clear();
        bestCost = 0;
        return;
    }
    int maxMask=1 << n;
    dpTable.resize(graphNumNodes);
    parent.resize(graphNumNodes);
    for(int i=0;i<graphNumNodes;i++){
        dpTable[i].assign(maxMask,INF);
        parent[i].assign(maxMask,-1);
    }

    dpTable[startNode][0]=0;

    for(int mask =0;mask<maxMask;mask++){
        for(int u=0;u<graphNumNodes;u++){
            if(dpTable[u][mask]==INF)
            {
                continue;
            }
            for(int i=0;i<n;i++){
                if(mask & (1<<i))
                {
                    continue;
                }
                int v=destinations[i];
                double dist=graphDistMatrix[u][v];
                int newMask=mask|(1<<i);
                if(dpTable[u][mask]+dist <dpTable[v][newMask]){
                    dpTable[v][newMask]=dpTable[u][mask]+dist;
                    parent[v][newMask]=u;
                }

            }
        }
    }
    //find best node that returns to start
    int fullmask=maxMask-1;
    int endNode=-1;
    double minCost=INF;

    for(int i=0;i<n;i++){
        int nod=destinations[i];
        double cost=dpTable[nod][fullmask]+graphDistMatrix[nod][startNode];
        if(cost<minCost){
            minCost=cost;
            endNode=nod;
        }
    }
    //reconstruct path
    optimalPath.clear();
    if(endNode==-1)
    {
        bestCost = INF;
        return;
    }
    bestCost = minCost;
    int mask=maxMask-1;
    int current=endNode;
    vector<int>reversePath;

    while(mask !=0 && current != -1){
        reversePath.push_back(current);
        int prev=parent[current][mask];

        for(int i=0;i<n;i++){
            if(destinations[i]==current && (mask & (1<<i)))
            {
                mask =mask ^ (1<<i);
                break;
            }
        }
        current=prev;
    }

    optimalPath.push_back(startNode);
    int sz=reversePath.size();
    for(int i=sz-1;i>=0;i--){
        optimalPath.push_back(reversePath[i]);
    }

    // Close the tour by returning to the depot/start
    optimalPath.push_back(startNode);

}

void dijkstraShortestPaths(int source,double distances[],int previous[])
{

    bool visited[MAX_NODES]={false};

    for(int i=0;i<graphNumNodes;i++)
    {
        distances[i]=INF;
        previous[i]=-1;
    }
    distances[source]=0;

    priority_queue<pair<double,int>,vector<pair<double,int>>,greater<pair<double,int>>> pq;
    pq.push({0,source});

    while(!pq.empty())
    {
        int u=pq.top().second;
        double dist=pq.top().first;
        pq.pop();

        if(visited[u]){
            continue;
        }
        visited[u]=true;

        for(int v:graphAdjList[u]){
            double edgeWeight=graphDistMatrix[u][v];
            double newDist=distances[u]+edgeWeight;

            if(newDist < distances[v]){
                distances[v]=newDist;
                previous[v]=u;
                pq.push({newDist,v});
            }
        }
    }
}

void yenKShortestPaths(int source,int destination,int k,
                      vector<vector<int>>&kPaths,vector<double>& kDistances)
 {

    kPaths.clear();
    kDistances.clear();

    double distances[MAX_NODES];
    int previous[MAX_NODES];
    dijkstraShortestPaths(source, distances, previous);

    if(distances[destination] == INF)return ;

    vector<int>firstPath;
    int current=destination;
    while(current!=-1)
    {
        firstPath.push_back(current);
        current=previous[current];

    }
    reverse(firstPath.begin(),firstPath.end());

    kPaths.push_back(firstPath);
    kDistances.push_back(distances[destination]);

    set<pair<double,vector<int>>> candidatePaths;

    for(int k_idx=1;k_idx<k; k_idx++)
    {
        vector<int>prevPath=kPaths[k_idx -1] ;

        for(int i=0;i<prevPath.size()-1;i++)
        {

            int spurNode=prevPath[i];
            vector<int> rootPath;
            for (int j = 0; j <= i; j++)
             {
                rootPath.push_back(prevPath[j]);
            }

            // Temporarily remove edges
            map<pair<int, int>, double> removedEdges;
            
            for (const auto& path : kPaths) 
            {
                if (path.size() > i && equal(rootPath.begin(), rootPath.end(), path.begin())) {
                    if (i + 1 < path.size())
                     {
                        int u = path[i];
                        int v = path[i + 1];
                        removedEdges[{u, v}] = graphDistMatrix[u][v];
                        graphDistMatrix[u][v] = INF;
                    }
                }
            }

            double spurDistances[MAX_NODES];
            int spurPrevious[MAX_NODES];
            dijkstraShortestPaths(spurNode,spurDistances,spurPrevious);
            if(spurDistances[destination] !=INF)
            {
                vector<int>spurPath;
                int curr=destination;
                while(curr!=-1 && curr!=spurNode)
                {
                    spurPath.push_back(curr);
                    curr=spurPrevious[curr];
                }
                spurPath.push_back(spurNode);
                reverse(spurPath.begin(),spurPath.end());

                vector<int>totalpath=rootPath;
                for (int j = 1; j < spurPath.size(); j++)
                {
                totalpath.push_back(spurPath[j]);
                }
                double totaldist=0;
                for(int j=0;j<totalpath.size()-1;j++)
                {
                    totaldist += graphDistMatrix[totalpath[j]][totalpath[j+1]];
                }
                candidatePaths.insert({totaldist,totalpath});

            }

            for(auto  &edge:removedEdges)
            {
                int u=edge.first.first;
                int v=edge.first.second;
                double dd=edge.second;
                graphDistMatrix[u][v]=dd;
            }

        }
        if(candidatePaths.empty())
        {
            break;
        }

        auto best=candidatePaths.begin();
        kPaths.push_back(best->second);
        kDistances.push_back(best->first);
        candidatePaths.erase(best);
    }

    for(int i=0;i<kPaths.size();i++)
    {
        cout<<"\nPath "<<(i+1)<<": ";
        for(int node:kPaths[i]){
            cout<<getNodeName(node)<<"-> ";
        }
        cout<<" | Distance: "<<fixed<<setprecision(3)<<kDistances[i]<<" km\n";
    }

}



// Rebuilds a node-by-node route using the next-matrix from Floyd–Warshall.
void expandTourWithNext(const vector<int>& tour,
                    int allPairsNext[MAX_NODES][MAX_NODES], vector<int>& expanded) 
{
    expanded.clear();
    if (tour.size() < 2) return;

    expanded.push_back(tour[0]);
    for (int idx = 0; idx + 1 < tour.size(); idx++) {
        int u = tour[idx];
        int v = tour[idx + 1];

        if (allPairsNext[u][v] == -1) 
        {
            expanded.clear();
            return;
        }
        int cur = u;
        while (cur != v)
         {
            cur = allPairsNext[cur][v];
            expanded.push_back(cur);
        }
    }
}  

void optimieDeliveries(){
    cout<<"\n==================================================================\n";
    cout<<"                  STARTING DELIVERY OPTIMIZATION PROCESS            \n";

   cout<<"\n==================================================================\n";

   //Extract unique destinations
   set<int> uniqueDestinations;
    for (int i = 0; i < numPackages; i++)
     {
        uniqueDestinations.insert(packageDestination[i]);
    }
     vector<int>destinations;
    for(auto dest:uniqueDestinations)
    {
        destinations.push_back(dest);
    }

    cout<<"Planning routes to "<<destinations.size() << "destination..\n";
    cout<<"Floyd-Warshall algorithm..\n";

        // Preserve original edge-weight matrix for algorithms that need raw edges later.
        double originalDist[MAX_NODES][MAX_NODES];
        for (int i = 0; i < graphNumNodes; i++)
         {
            for (int j = 0; j < graphNumNodes; j++) 
            {
                originalDist[i][j] = graphDistMatrix[i][j];
            }
        }

    double allPairsDistances[MAX_NODES][MAX_NODES];
    int allPairsNext[MAX_NODES][MAX_NODES];
    floydWarshallAllPairs(allPairsDistances,allPairsNext);

    // Use shortest-path distances for the TSP solver
    for (int i = 0; i < graphNumNodes; i++) 
    {
        for (int j = 0; j < graphNumNodes; j++)
         {
            graphDistMatrix[i][j] = allPairsDistances[i][j];
        }
    }

    //cout<<"Solving TSP with Dynamic Programming.\n";

    vector<int>optimalPath;
    int startNode;
    if (depotNode >= 0 && depotNode < graphNumNodes) {
    startNode = depotNode;
    }
   else {
    startNode = 0;
   }

      if (destinations.size() <= 20) {
        int n = destinations.size();
        int maxMask = 1 << n;
        vector<vector<double>> dpTable(MAX_NODES, vector<double>(maxMask));
        vector<vector<int>> parent(MAX_NODES, vector<int>(maxMask));
        double tourCost = INF;
        tspsolve(startNode, destinations, dpTable, parent, optimalPath, tourCost);

        if (tourCost >= INF/2 || optimalPath.size() <= 1) {
            cout << "No feasible tour found.\n";
        } 
        else {
            cout << "\nOptimal tour (depot returns included):\n  ";
            for (int i = 0; i < optimalPath.size(); i++) {
                cout << getNodeName(optimalPath[i]);
                if (i + 1 < optimalPath.size()) {
                    cout << " -> ";
                }
            }
            cout << "\nTotal distance: " << fixed << setprecision(3) << tourCost << " km\n";

            vector<int> expandedRoute;
            expandTourWithNext(optimalPath, allPairsNext, expandedRoute);
            if (!expandedRoute.empty()) {
                cout << "Expanded route (edge-by-edge):\n  ";
                for (size_t i = 0; i < expandedRoute.size(); i++)
                 {
                    cout << getNodeName(expandedRoute[i]);
                    if (i + 1 < expandedRoute.size()) {
                        cout << " -> ";
                    }
                }
                cout << "\n";
            }
        }
        
    }


    // Restore original edge weights so Yen's K-shortest paths sees true edge costs.
    for (int i = 0; i < graphNumNodes; i++) {
        for (int j = 0; j < graphNumNodes; j++) {
            graphDistMatrix[i][j] = originalDist[i][j];
        }
    }


    int targetDestination=destinations[0];
    vector<vector<int>> yenPaths;
    vector<double> yenDistances;
    yenKShortestPaths(startNode, targetDestination, K_SHORTEST_PATHS, yenPaths, yenDistances);
 

    
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

    // Select depot (start) node
    cout << "\nChoose depot/start city index (0-" << (numNodes - 1) << "): ";
    int depotInput;
    cin >> depotInput;
    if (depotInput >= 0 && depotInput < numNodes) {
        depotNode = depotInput;
    } else {
        cout << "[ERROR] Invalid depot index. Defaulting to 0.\n";
        depotNode = 0;
    }
    cin.ignore();

    cout << "\nEnter names for each city:\n";
    for (int i = 0; i < numNodes; i++) {
        cout << "  City " << i << ": ";
        string name;
        getline(cin, name);
        string finalName = name.empty() ? ("City_" + to_string(i)) : name;
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

        if (from >= 0 && from < graphNumNodes && to >= 0 && to < graphNumNodes && distance > 0) {
            addBidirectionalEdge(from, to, distance, 0);
            cout << "    [OK] Road added: " << getNodeName(from) << " <-> "
                 << getNodeName(to) << " (" << distance << " km)\n";
        } else {
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
        optimieDeliveries();
    } 

}


