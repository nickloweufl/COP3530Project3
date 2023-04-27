#include "TreeMap.h"

// Emscripten includes
#include <emscripten/bind.h>
using namespace emscripten;

// I'm not entirely sure why but placing definition for the class constructor 
// at the top of the file causes a compilation error.

// Increments counters for cooresponding gender.
void CovidTreeMap::updateGender(int gender, vector<int> &genders) {
    genders[gender] += 1;
};


CovidTreeMap::CovidTreeMap() { 
    this->Map = nullptr; 
    this->FIPS = nullptr;
}

CovidTreeMap::~CovidTreeMap() {
    // Deletes every age map first
    for (auto age_map: this->age_map_pointers){
        delete age_map;
    }
    // Then deletes the Treemap and FIPS map
    delete this->Map;
    delete this->FIPS;
}


// Loads fips map using values from "Fl_County_FIPS.csv" and stores it in FIPS
RBTree<string,int>* CovidTreeMap::fipsMapHelper () {
    RBTree<string,int>* fipstree = new RBTree<string,int>;
    string county; int FIPS;
    vector<string> row;
    string line, cell;
    // "Fl_County_FIPS.csv" has county name and ANSI/FIPS id as its columns
    fstream fips_file ("data_files/FL_County_FIPS.csv", ios::in);
    if (fips_file.is_open()) {
        while (getline(fips_file, line)) {
            row.clear();
            stringstream ss(line);
            while (getline(ss, cell, ',')) {
                row.push_back(cell);
            }
            county = row[0];
            FIPS = stoi(row[1]);
            fipstree->insert(county, FIPS);
        }
    } else {
        cout << "Could not open FIPS file." << endl;
        delete fipstree;
        return nullptr;
    }
    return fipstree;

}
// Attempts to load FIPS map and prints error if failed
void CovidTreeMap::loadFipsMap() {
    auto fipsmap = fipsMapHelper();
    if (fipsmap == nullptr) {
        cout << "FIPS failed to load." << endl;
    } else {
        this->FIPS = fipsmap;
    }
}

// Loads treemap and stores it in Map
RBTree<string,RBTree<int,vector<int>>*>* CovidTreeMap::loadTreemapHelper() {
    RBTree<string,RBTree<int,vector<int>>*>* data = new RBTree<string,RBTree<int,vector<int>>*>;
    unordered_set<RBTree<int,vector<int>>*> age_gender_map_set;
    string county; int age, gender;
    vector<string> row;
    string line, cell;
    // "FL_COVID_Data.csv" has three columns that are used county, age, and gender
    fstream covid_file ("data_files/FL_COVID_Data.csv", ios::in);
    if (covid_file.is_open()) {
        while (getline(covid_file, line)) {
            row.clear();
            
            stringstream ss(line);
            while (getline(ss, cell, ',')) {
                row.push_back(cell);
            }
            county = row[0]; // County name
            age = stoi(row[1]); // Age of patient
            gender = (row[2] == "Male") ? 0 : 1; // Gender
            if (!data->isKeyInMap(county)) { // If county is not in map
                RBTree<int,vector<int>>* age_gender = new RBTree<int,vector<int>>; // creates an age-gender map
                vector<int> genders (2,0);
                updateGender(gender, genders); // Increments appropriate gender tally
                age_gender->insert(age, genders); // stores gender vector by age in age_gender map
                age_gender_map_set.insert(age_gender); // Stores pointer to set for deconstructor
                data->insert(county, age_gender);
            } else { // if the county is in the map
                auto countyMap = data->get(county); 
                if(!(countyMap->value)->isKeyInMap(age)) { // checks if age is in county's age-gender map
                    // if it isn't, adds node with age key to age-gender map
                    vector<int> genders (2,0); 
                    genders[gender] += 1;
                    (countyMap->value)->insert(age,genders);
                } else { // if it is, update appropriate gender tally
                    updateGender(gender, ((countyMap->value)->get(age))->value);
                }
            }
            
        }
    } else {
        cout << "Could not open Covid data." << endl;
        delete data;
        return nullptr;
    }
    this->age_map_pointers = age_gender_map_set;
    return data;
}

// Attempts to load treemap and prints error if failed
void CovidTreeMap::loadTreeMap() {
    if (this->Map != nullptr) {
        cout << "Treemap already loaded" << endl;
        return;
    }
    auto treemap = loadTreemapHelper();
    if (treemap == nullptr) {
        cout << "Treemap failed to load." << endl;
    } else {
        this->Map = treemap;
    }
};

/* Writes Treemap data to "chart_data_tm.csv" within age range (loBound to upBound)
   The file will have the columns county_name,id,county,in_age_range,male,female" 
   - county_name = name of county
   - id = FIPS id
   - county = number of cases in county
   - in_age_range = number of cases in age range in county
   - male = number of cases of male patients in in_age_range
   - female = number of cases of female patients in in_age_range */
void CovidTreeMap::writeCSV(int loBound, int upBound) {
    if (this->FIPS == nullptr) { // Requires FIPS
        cout << "Write failed. FIPS not loaded." << endl; 
    } else if (this->Map == nullptr) { // and Treemap to be loaded
        cout << "Write failed. Treemap not loaded." << endl;
    } else { // If both required maps are loaded
        ofstream writer("data_files/chart_data_tm.csv"); // Attempts to open csv file.
        if (!writer) {
            cout << "There was an error opening csv file for output" << endl;
            return;
        }
        // Adds headers to top of file.
        string headers= "county_name,id,county,in_age_range,male,female";
        writer << headers << endl;

        
        auto counties = this->Map->getAll(); // Gets all county-age pairs
        string county_name;
        int id, county_count, in_age_range, male, female;
        for (auto county: counties) {
            id = county_count = in_age_range = male = female = 0;
            county_name = county.first;
            id = (FIPS->get(county.first))->value;
            county_count = county.second->getAccessCount();
            auto ages = (county.second)->getInRange(loBound, upBound); // Gets every age node in age range
            for (auto age: ages) { // Sums all of the tallies in age range for each gender
                male += age.second[0]; 
                female += age.second[1];
            }
            in_age_range = (male + female); // The age range is just the sum of all male and female patients.

            // Writes line to file
            writer << county_name << "," << id << "," << county_count << "," << in_age_range << "," << male << "," << female << "\n";
        }
        writer.close();
    }
}

// Emscripten binding
EMSCRIPTEN_BINDINGS(covid_tree_map) {
    class_<CovidTreeMap>("CovidTreeMap")
        .constructor<>()
        .function("writeCSV", &CovidTreeMap::writeCSV)
        .function("loadTreeMap", &CovidTreeMap::loadTreeMap)
        .function("loadFipsMap", &CovidTreeMap::loadFipsMap);
}