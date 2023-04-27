#include "HashMap.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <emscripten/bind.h>

using namespace std;
using namespace emscripten;

// This is meant to store the COVID data and write it to an CSV.
class CovidHashMap {
private:
    HashMap map;
    vector<string> counties; // There is no way to actually get the counties from the original map structure by itself.
public:
    CovidHashMap () {
        this->map = HashMap();
        vector<string> vec{"Alachua","Baker","Bay","Bradford","Brevard","Broward",
        "Calhoun","Charlotte","Citrus","Clay","Collier","Columbia", "DeSoto","Dixie",
        "Duval","Escambia","Flagler","Franklin", "Gadsden","Gilchrist","Glades","Gulf",
        "Hamilton","Hardee","Hendry","Hernando","Highlands","Hillsborough","Holmes",
        "Indian River","Jackson","Jefferson","Lafayette","Lake", "Lee","Leon","Levy",
        "Liberty","Madison","Manatee","Marion","Martin","Miami-Dade","Monroe","Nassau",
        "Okaloosa","Okeechobee","Orange","Osceola","Palm Beach","Pasco","Pinellas","Polk",
        "Putnam","St. Johns","St. Lucie","Santa Rosa","Sarasota", "Seminole","Sumter",
        "Suwannee","Taylor","Union","Volusia", "Wakulla","Walton","Washington"};
        this->counties = vec;
    }
    void loadHashMap();
    void writeCSV(int loBound, int upBound);
};

void CovidHashMap::loadHashMap() {
    string county, gender; int age;
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
            gender = row[2]; // Gender
            this->map.insert(county, age, gender);
        }
    } else {
        cout << "Failed to load COVID Data." << endl;
    }
}


void CovidHashMap::writeCSV(int loBound, int upBound) {
    ofstream writer("data_files/chart_data_hm.csv"); // Attempts to open csv file.
    if (!writer) {
        cout << "There was an error opening csv file for output" << endl;
        return;
    }
    // Adds headers to top of file.
    string headers= "county_name,id,county,in_age_range,male,female";
    writer << headers << endl;

    string county_name;
    int id, county_count, in_age_range, male, female;
    vector<int> male_range_count(67, 0);
    vector<int> female_range_count(67, 0);
    for (int i = loBound; i < upBound; i++) {
        vector<int> male_count = map.countyAgeGenderCounts(i, "Male");
        vector<int> female_count = map.countyAgeGenderCounts(i, "Female");
        for (int j = 0; j < 67; j++ ) {
            male_range_count[j] += male_count[j];
            female_range_count[j] += female_count[j];
        }
    }
    vector<int> county_total_counts = map.countyCounts();
    for (int i = 0; i < 67; i++) {
        county_name = counties[i];
        id = map.county_to_code(counties[i]);
        county_count = county_total_counts[i];
        male = male_range_count[i];
        female = female_range_count[i];
        in_age_range = male + female;
        // Writes line to file
        writer << county_name << "," << id << "," << county_count << "," << in_age_range << "," << male << "," << female << "\n";
    }
    writer.close();
}

// Emscripten binding
EMSCRIPTEN_BINDINGS(covid_hash_map) {
    class_<CovidHashMap>("CovidHashMap")
        .constructor<>()
        .function("writeCSV", &CovidHashMap::writeCSV)
        .function("loadHashMap", &CovidHashMap::loadHashMap);
}