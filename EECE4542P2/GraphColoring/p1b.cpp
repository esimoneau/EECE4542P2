// Code to read graph instances from a file.  Uses the Boost Graph Library (BGL).
// Project 2b: Solving map coloring using greedy algorithm
// Authors: Patricia Gavelek, Emily Simoneau

#include <iostream>
#include <limits.h>
#include "d_except.h"
#include <fstream>
#include <time.h>
#include <vector>

#include <boost/graph/adjacency_list.hpp>

#define LargeValue 99999999

using namespace std;
using namespace boost;

int const NONE = -1;  // Used to represent a node that does not exist

struct VertexProperties;
struct EdgeProperties;

typedef adjacency_list<vecS, vecS, undirectedS, VertexProperties, EdgeProperties> Graph;

int CONFLICTS;
ofstream solution;

struct VertexProperties
{
    pair<int,int> cell; // maze cell (x,y) value
    Graph::vertex_descriptor pred;
    bool visited;
    bool marked;
    int weight;
    int color;
};

// Create a struct to hold properties for each edge
struct EdgeProperties
{
    int weight;
    bool visited;
    bool marked;
};

void initializeGraph(Graph &g, ifstream &fin)
// Initialize g using data from fin.
{
    int n, e;
    int j,k;
    
    fin >> n >> e;
    Graph::vertex_descriptor v;
    
    // Add nodes.
    for (int i = 0; i < n; i++)
        v = add_vertex(g);
    
    for (int i = 0; i < e; i++)
    {
        fin >> j >> k;
        add_edge(j,k,g);  // Assumes vertex list is type vecS
    }
}

void setNodeWeights(Graph &g, int w)
// Set all node weights to w.
{
    pair<Graph::vertex_iterator, Graph::vertex_iterator> vItrRange = vertices(g);
    
    for (Graph::vertex_iterator vItr= vItrRange.first; vItr != vItrRange.second; ++vItr)
    {
        g[*vItr].weight = w;
    }
}

// prints arrays
void printArray(int arr[], int size) {
    for ( int i = 0; i < size; i++ ) {
        cout << arr[i] << ' ';
    }
    cout << endl;
}

// copy one array into another
void copyArray(int arrs[], int arrd[], int size) {
    for ( int i = 0; i < size; i++ ) {
        arrd[i] = arrs[i];
    }
}

// initialize an array with 0's
void initArray(int arr[], int size) {
    for (int j = 0; j < size; j++)
        arr[j] = 0;
}

// Find the spot in array with the minimum value
int findMin(int arr[], int size) {
    int min =1;
    int con = arr[0];
    for (int j = 0; j < size; j++) {
        if (arr[j] <= con) {
            con =arr[j];
            min = j + 1;
        }
    }
    return min;
}

//prints solution to screen and file
void printSolution(Graph &g, int numConflicts) {
    //Grab all of the vertices
    pair<Graph::vertex_iterator, Graph::vertex_iterator> vItrRange = vertices(g);
    int counter = 1;
    cout << "Number of conflicts: " << CONFLICTS << endl;
    solution << "Number of conflicts: " << CONFLICTS << endl;
    //Cycle through all of the vertices
    for (Graph::vertex_iterator vItr = vItrRange.first; vItr != vItrRange.second; ++vItr){
        cout << counter << " - " << g[*vItr].color << "\n";
        solution << counter << " - " << g[*vItr].color << "\n";
        counter++;
    }
}

void clearVisited(Graph &g){
    // Get a pair containing iterators pointing the beginning and end of the
    // list of nodes
    pair<Graph::vertex_iterator, Graph::vertex_iterator> vItrRange = vertices(g);
    // Loop over all nodes in the graph setting the visited value to false
    for (Graph::vertex_iterator vItr= vItrRange.first; vItr != vItrRange.second; ++vItr){
        g[*vItr].visited = false;
    }
    
}

//Determines given a graph how many instances of two adjacent nodes sharing the same color there are
int returnConflicts(Graph &g) {
    int numConflicts = 0;
    //Clear all nodes marked as visited in the graph
    clearVisited(g);
    //Grab all of the vertices
    pair<Graph::vertex_iterator, Graph::vertex_iterator> vItrRange = vertices(g);
    //Cycle through all of the vertices
    for (Graph::vertex_iterator vItr = vItrRange.first; vItr != vItrRange.second; ++vItr){
        //Grab the vertex descriptor of the current node
        Graph::vertex_descriptor v = *vItr;
        //If the current node has not been visited
        if (!g[v].visited){
            //Mark the current node as visited
            g[v].visited = true;
            //Grab all of the adjacent nodes to the current node
            pair<Graph::adjacency_iterator, Graph::adjacency_iterator> aItrRange = adjacent_vertices(v, g);
            //Cycle through all adjacent vertices
            for (Graph::adjacency_iterator aItr = aItrRange.first; aItr != aItrRange.second; ++aItr) {
                //Grab the vertex descriptor of the current node
                Graph::vertex_descriptor a = *aItr;
                //If this current adjacent node has not been checked for conflicts already and its color conflicts with the current node's color
                if (!g[a].visited && g[a].color == g[v].color) {
                    //Increment colors
                    numConflicts++;
                }
            }
        }
    }
    return numConflicts;
}

//find color with least conflicts for node v
int getBestColor(Graph::vertex_descriptor v, Graph &g, int numColors) {
    //creates array for neighbor node colors
    int color [numColors];
    initArray(color, numColors);

    pair<Graph::adjacency_iterator, Graph::adjacency_iterator> aItrRange = adjacent_vertices(v, g);
    //Cycle through all adjacent vertices
    for (Graph::adjacency_iterator aItr = aItrRange.first; aItr != aItrRange.second; ++aItr) {
        //Grab the vertex descriptor of the current node
        Graph::vertex_descriptor a = *aItr;
        //add number of conflicts for each color in the array
        if (g[a].color != 0)
            color[(g[a].color -1)] += 1;
    }
    return findMin(color, numColors);
}

//greedy coloring, chooses color with least amount of conflicts for each node
int greedyColoring(Graph &g, int numColors, int t) {
    //initialize clock
    float startTime = clock()/ CLOCKS_PER_SEC;
    
    //Grab all of the vertices
    pair<Graph::vertex_iterator, Graph::vertex_iterator> vItrRange = vertices(g);
    
    //initialize all vertices to no color
    for (Graph::vertex_iterator vItr = vItrRange.first; vItr != vItrRange.second; ++vItr){
        //Grab the vertex descriptor of the current node
        Graph::vertex_descriptor v = *vItr;
        g[v].color = 0;
    }
    
    //initially sets conflicts
    CONFLICTS = num_edges(g);
    
    //Cycles through each node
    for (Graph::vertex_iterator vItr = vItrRange.first; vItr != vItrRange.second; ++vItr){
        //Check that the current runtime is not past the specified max time
        if ((float)(clock() - startTime) / CLOCKS_PER_SEC >= t) {
            CONFLICTS = returnConflicts(g);
            return CONFLICTS;
        }
        Graph::vertex_descriptor v = *vItr;
        //Color current node with the least conflicts color
        g[v].color = getBestColor(*vItr, g, numColors);
        
        //sets conflicts to actual number
        CONFLICTS = returnConflicts(g);
    }
    return CONFLICTS;
}

int main()
{
    solution.open("results192-8.output");
    ifstream fin;
    string fileName = "color192-8.input";
    // Read the name of the graph from the keyboard or
    // hard code it here for testing.
    
    fin.open(fileName.c_str());
    if (!fin)
    {
        cerr << "Cannot open " << fileName << endl;
        exit(1);
    }
    
    try
    {
        cout << "Reading graph" << endl;
        Graph g;
        int numColors;
        int numConflicts = -1;
        fin >> numColors;
        initializeGraph(g,fin);
        fin.close();
        
        cout << "Num nodes: " << num_vertices(g) << endl;
        cout << "Num edges: " << num_edges(g) << endl;
        cout << endl;
        
        //cout << g;
        
        numConflicts = greedyColoring(g, numColors, 600);
        //Grab all of the vertices

        printSolution(g, CONFLICTS);
        solution.close();
        
    }
    catch (indexRangeError &ex)
    {
        cout << ex.what() << endl; exit(1);
    }
    catch (rangeError &ex)
    {
        cout << ex.what() << endl; exit(1);
    }
}
