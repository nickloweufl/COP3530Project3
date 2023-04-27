function chartCSVStringToObj (CSV) {
   return d3.csvParse(CSV, (d) => {
        return {
            id: d.id,
            county_count: +d.county,
            age_count: +d.in_age_range,
            male_count: +d.male,
            female_count: +d.female
    }})
}

// Tree map functions
function loadCovidTreeMap () {
    if (typeof window.Treemap == 'undefined') { // Checks if tree map exists in memory
        var Treemap = new Module.CovidTreeMap();
        let start = performance.now(); 
        Treemap.loadTreeMap();
        let duration = performance.now() - start;
        window.TreeLoadTime = duration;
        Treemap.loadFipsMap();
        window.Treemap = Treemap;
    } else {
        alert("Tree map already loaded.");
    }
}

function searchTreeAgeRange(loAge, hiAge) {
    if (typeof window.Treemap != "undefined") { 
        let start = performance.now();
        window.Treemap.writeCSV(loAge, hiAge);
        let duration = performance.now() - start;
        window.TreeSearchTime = duration;
        var covid_data = FS.readFile("/data_files/chart_data_tm.csv"); // Retrieves virtual file as utf-8 string
        var covid_csv = new TextDecoder("utf-8").decode(covid_data) // Decodes string
        updateChart(chartCSVStringToObj(covid_csv)); // Updates Chart after converting csv string into js object
    } else {
        alert("Load tree map first")
    }
}

// Same as above but for hash map
function loadCovidHashMap () {
    if (typeof window.Hashmap == 'undefined') {
        var Hashmap = new Module.CovidHashMap();
        let start = performance.now();
        Hashmap.loadHashMap();
        let duration = performance.now() - start;
        window.HashLoadTime = duration;
        window.Hashmap = Hashmap;
    } else {
        alert("Hash map already loaded.");
    }
}

function searchHashAgeRange(loAge, hiAge) {
    if (typeof window.Hashmap != "undefined") {
        let start = performance.now();
        window.Hashmap.writeCSV(loAge, hiAge);
        let duration = performance.now() - start;
        window.HashSearchTime = duration;
        var covid_data = FS.readFile("/data_files/chart_data_hm.csv");
        var covid_csv = new TextDecoder("utf-8").decode(covid_data)
        updateChart(chartCSVStringToObj(covid_csv));
    } else {
        alert("Load hash map first")
    }
}

// Data structure selection for radio buttons
function loadDataStructure() {
    if (document.getElementById("treemap").checked == true) {
        loadCovidTreeMap();
        document.getElementById("TreeMapLoadTime").innerText = TreeLoadTime.toPrecision(4);
    } else if (document.getElementById("hashmap").checked == true) {
        loadCovidHashMap();
        document.getElementById("HashMapLoadTime").innerText = HashLoadTime.toPrecision(4);
    } else {
        alert("Select a map type.");
    }
}

// Search data in age range
function SearchDataInAgeRange() {
    let low = parseInt(document.getElementById('lo_age').value);
    let high = parseInt(document.getElementById('hi_age').value);
    if (isNaN(low) || isNaN(high)) {
        alert("Enter an upper and lower age limit for search.");
        return;
    }
    if (document.getElementById("treemap").checked == true &&
            typeof window.Treemap != "undefined") {
        searchTreeAgeRange(low, high);
        document.getElementById("TreeMapSearchTime").innerText = TreeSearchTime.toPrecision(4);
        document.getElementById("map-type").innerText = "Tree map.";
        document.getElementById("lower_age").innerText = low;
        document.getElementById("higher_age").innerText = high;
    } else if (document.getElementById("hashmap").checked == true && 
            typeof window.Hashmap != "undefined") {
        searchHashAgeRange(low, high);
        document.getElementById("HashMapSearchTime").innerText = HashSearchTime.toPrecision(4);
        document.getElementById("map-type").innerText = "Hash map.";
        document.getElementById("lower_age").innerText = low;
        document.getElementById("higher_age").innerText = high;
    } else {
        alert("Load a map before searching.");
    }
}