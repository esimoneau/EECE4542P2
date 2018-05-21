// Project 2a: Solving knapsack with Greedy Algorithm
//NOTE: does not guarantee an optimal solution

#include <iostream>
#include <limits.h>
#include <list>
#include <fstream>
#include <queue>
#include <vector>
#include <time.h>

using namespace std;

#include "d_except.h"
#include "d_matrix.h"
#include "knapsack.h"

// Finds the next best unselected object based first on highest value then on lowest weight
int nextHighest(knapsack &k) {
    // Initialize variable for keeping track of the highest ratio
    int highestRatio = 0;
    // Cycle through the objects and find the highest valued object
    for (int obj = 0; obj < k.getNumObjects(); obj++) {
        // If the current object is unselected and has the highest value to weight ratio update the variable
        if(!k.isSelected(obj) && (((k.getValue(obj) / k.getCost(obj)) > highestRatio) || (((k.getValue(obj) / k.getCost(obj)) == highestRatio) && (k.getValue(obj) > k.getValue(highestRatio))))){
            highestRatio = obj;
        }
    }
}

// Sorts the objects for a knapsack instance
int[] sortObjects(knapsack &k) {
    int objects[k.getNumObjects()];
    for (int obj = 0; obj < k.getNumObjects(); obj++) {
        int high = nextHighest(k);
        objects[obj] = high;
        k.select(high);
    }
    return objects;
}

// Implements a greedy approach to solving the 0-1 knapsack problem
// Highest value to weight ratio takes precedence. Highest value is a secondary consideration given equal ratios
void greedyKnapsack(knapsack &k, int t) {
    int startTime = clock() / CLOCKS_PER_SECOND;
    knapsack copy(k);
    int sorted = sortObjects(copy);
    for (int x = 0; x < k.getNumObjects(); x++) {
        if (clock() / CLOCKS_PER_SECOND - startTime > t) {
            return k;
        }
        if (k.getCost()+k.getCost(sorted[x])) <= k.getCostLimit()) {
            k.select(sorted[x]);
        }
        else {
            break;
        }
    }
}

int main()
{
   char x;
   ifstream fin;
   stack <int> moves;
   string fileName;
   
   // Read the name of the file from the keyboard or
   // hard code it here for testing.
   
   // fileName = "knapsack16.input";

   cout << "Enter filename" << endl;
   cin >> fileName;
   
   fin.open(fileName.c_str());
   if (!fin)
   {
      cerr << "Cannot open " << fileName << endl;
      exit(1);
   }

   try
   {
      cout << "Reading knapsack instance" << endl;
      knapsack k(fin);

      greedyKnapsack(k, 600);

      cout << endl << "Best solution" << endl;
      k.printSolution();
      
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

