// Map.h

/* This is a hash based map that uses separate chaining. The map is effectively sorted by making the hash function map 
the counties (key) to the indices and then storing them into separate buckets (arrays) that are also hashed maps. Those maps hash using the ages (keys)
to the indices. Gender is then stored as a final array of two elements. Therefore, our map is really a map of maps of tuples based on separate chaining hashing.

The hashing uses ansi codes for the counties in case that functionality is needed for the interface.

Functions are included that allow searching by various criteria. 

*/
#pragma once
#include <vector>
#include <string>

using namespace std;

class HashMap
{

    int _total_count = 0;
    int map[67][119][2]; // The map itself as county (0-66), age(0-118), gender(M/F)

    int code_to_index(int county_code);
    // int county_to_code(string county); 
    int hash(string county);

public:

    HashMap();
    void insert(string county, int age, string sex);
    
    int county_to_code(string county); // Jonathan here. I had to make this method public last minute.

    // Functions for returning data
     int totalCases(); // Total number of cases recorded
     int countyCaseCount(string county); // total cases in a particular county
     vector<int> countyCounts(); // a vector containing the cases counts of each county
     vector<int> countyAgeCounts(int age); // a vector containing the case counts for a particular age for each county 
     vector<int> countyAgeGenderCounts(int age, string sex); // a vector containing case counts for given age, gender, for each county
     int genderCount(string sex); // returns number of all cases for a particular gender for the whole state
     int genderCountyCount(string county, string sex); // number of cases for a particular gender for a given county
};

/*******Counts********/
int HashMap::totalCases()
{
    return _total_count;
}
int HashMap::countyCaseCount(string county)
{
    int code = hash(county);
    int sum = 0;
    for (int i = 0; i < 119; i++)
        sum += map[code][i][0] + map[code][i][1]; // adds up male and female for [county][age]
    return sum;
}
vector<int> HashMap::countyCounts()
{
    vector<int> counts;
    for (int i = 0; i < 67; i++)
    {
        int sum = 0;
        for (int j = 0; j < 119; j++)
            sum += map[i][j][0] + map[i][j][1];
        counts.push_back(sum);
    }
    return counts;
}
vector<int> HashMap::countyAgeCounts(int age)
{
    vector<int> ages;
    int gender = 1;
    for (int i = 0; i < 67; i++)
        ages.push_back(map[i][age][0] + map[i][age][1]);
    return ages;
}
int HashMap::genderCount(string sex)
{
    int sum = 0;
    int gender = 1;
    if (sex == "Male") {gender = 0;}
    for (int i = 0; i < 67; i++)
        for (int j = 0; j < 119; j++)
            sum += map[i][j][gender];
    return sum;
}
int HashMap::genderCountyCount(string county, string sex)
{
    int code = hash(county);
    int gender = 1;
    if (sex == "Male") {gender = 0;}
    int sum = 0;
    for (int j = 0; j < 119; j++)
            sum += map[code][j][gender];
    return sum;
}
vector<int> HashMap::countyAgeGenderCounts(int age, string sex)
{
    vector<int> ages;
    int gender = 1;
    if (sex == "Male") {gender = 0;}
    for (int i = 0; i < 67; i++)
        ages.push_back(map[i][age][gender]);
    return ages;
}

/********Map Functions********/
HashMap::HashMap()
{
    // Set up map with counties and ages
    for (int i = 0; i < 67; i++)
        for (int j = 0; j < 119; j++) // ages from 0 to 119
            {
                map[i][j][0] = 0; // male cases = 0
                map[i][j][1] = 0; // female cases = 0
            }
}
int HashMap::hash(string county)
{
    int code = county_to_code(county);
    return code_to_index(code);
}
void HashMap::insert(string county, int age, string sex)
{
    int county_index = hash(county); // Gets index as 0 through 66.
    int gender = 1; // initialize to female case
    if (sex == "Male") {gender = 0;} // Switch to male if needed
    map[county_index][age][gender] += 1; // Age will function as the index (i.e, the separate chaining of the original hash)
    _total_count += 1; // Add to total number of cases
}
int HashMap::code_to_index(int code) // Hashing for the top-level of map
{
    // This is effectively the hash function
    // County codes (for Florida) range from 12001 to 12133, so subtract by 12000 to get
    // 1 through 133, then subtract 1, for 0 through 132. Codes increment by 2, so then divide by 2.
    // Now codes are 0 through 66 for 67 counties. Repeats (i.e., cases in the same county) use chaining for the hash table. 
    // return (((code-12000)-1)/2);
    

    // Jonathan Here. There was an error in the dataset that completly broke this function last minute.
    // I'm going to try to leave this function as close as possible to how it was designed before
    if (code < 12025) {
        return (((code-12000)-1)/2);
    } else if (code < 12086) {
        return (((code-12000)-3)/2);
    } else if (code > 12086) {
        return (((code-12000)-1)/2);
    } else {
        return (((code-12000)-2)/2);
    }
}
int HashMap::county_to_code(string county)
{
    // List copied from fcc.gov
    vector<pair<int,string>> county_list =
    {
    {12001,"Alachua"},
    {12003,"Baker"},
    {12005,"Bay"},
    {12007,"Bradford"},
    {12009,"Brevard"},
    {12011,"Broward"},
    {12013,"Calhoun"},
    {12015,"Charlotte"},
    {12017,"Citrus"},
    {12019,"Clay"},
    {12021,"Collier"},
    {12023,"Columbia"},
    {12027,"DeSoto"},
    {12029,"Dixie"},
    {12031,"Duval"},
    {12033,"Escambia"},
    {12035,"Flagler"},
    {12037,"Franklin"},
    {12039,"Gadsden"},
    {12041,"Gilchrist"},
    {12043,"Glades"},
    {12045,"Gulf"},
    {12047,"Hamilton"},
    {12049,"Hardee"},
    {12051,"Hendry"},
    {12053,"Hernando"},
    {12055,"Highlands"},
    {12057,"Hillsborough"},
    {12059,"Holmes"},
    {12061,"Indian River"},
    {12063,"Jackson"},
    {12065,"Jefferson"},
    {12067,"Lafayette"},
    {12069,"Lake"},
    {12071,"Lee"},
    {12073,"Leon"},
    {12075,"Levy"},
    {12077,"Liberty"},
    {12079,"Madison"},
    {12081,"Manatee"},
    {12083,"Marion"},
    {12085,"Martin"},
    {12086,"Miami-Dade"}, // Jonathan here. Dade county was switched to Miami-Dade FIPS(12025 => 12086).
    {12087,"Monroe"},
    {12089,"Nassau"},
    {12091,"Okaloosa"},
    {12093,"Okeechobee"},
    {12095,"Orange"},
    {12097,"Osceola"},
    {12099,"Palm Beach"},
    {12101,"Pasco"},
    {12103,"Pinellas"},
    {12105,"Polk"},
    {12107,"Putnam"},
    {12109,"St. Johns"},
    {12111,"St. Lucie"},
    {12113,"Santa Rosa"},
    {12115,"Sarasota"},
    {12117,"Seminole"},
    {12119,"Sumter"},
    {12121,"Suwannee"},
    {12123,"Taylor"},
    {12125,"Union"},
    {12127,"Volusia"},
    {12129,"Wakulla"},
    {12131,"Walton"},
    {12133,"Washington"},
    };

    for (auto i : county_list)
    {
        if (i.second == county)
            return i.first;
    }
    return -1;
}