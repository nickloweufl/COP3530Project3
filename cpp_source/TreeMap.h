#pragma once
#include "RBTree.h"
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_set>
#include <chrono>
using namespace std;

class CovidTreeMap {
private:
    RBTree<string,RBTree<int,vector<int>>*>* Map; // This is actually a map of maps that contain vectors that hold counters for each gender
    RBTree<string,int>* FIPS; // This is a separate map for converting county names to FIPS codes
    // This is a bit haphazard, but this set holds references to the inner age maps for them to be deleted when the deconstructor is called.
    // This is a temporary measure until I figure out how to delete the values if they are dynamically allocated, if I
    // run out of time. I'm leaving it as is.
    unordered_set<RBTree<int,vector<int>>*> age_map_pointers; 
    void updateGender(int gender, vector<int> &genders); 
    RBTree<string,RBTree<int,vector<int>>*>* loadTreemapHelper(); // loads the tree map and returns a pointer to it
    RBTree<string,int>* fipsMapHelper (); // loads FIPS map and returns a pointer to it
public:
    CovidTreeMap();
    ~CovidTreeMap();
    void loadFipsMap (); // Same as above but for FIPS map
    void loadTreeMap(); // Calls helper and stores treemap to Map if successful, prints error if helper fails
    void writeCSV(int loBound, int upBound); // Writes the covid_data.csv
};

