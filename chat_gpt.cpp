#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

using namespace std;

// A struct to represent each cache entry
struct CacheEntry {
    bool valid;
    int tag;
    int counter;
    int address;
};

// A function to compute the tag bits for a given memory address
int computeTagBits(int address, int numIndexBits, int numOffsetBits) {
    return address >> (numIndexBits + numOffsetBits);
}

// A function to simulate a cache access and return whether it was a hit or miss
bool accessCache(int address, vector<vector<CacheEntry>>& cache, int numIndexBits, int numOffsetBits) {
    int index = (address >> numOffsetBits) & ((1 << numIndexBits) - 1);
    int tag = computeTagBits(address, numIndexBits, numOffsetBits);

    // Search for a matching tag and address in the cache
    for (int i = 0; i < cache[index].size(); i++) {
        if (cache[index][i].valid && cache[index][i].tag == tag && cache[index][i].address == address) {
            // Hit!
            cout << "HIT!!" << endl;
            cache[index][i].counter = 0;
            return true;
        }
    }

    // Miss - update an existing entry with the same tag or replace the oldest entry
    int oldestEntryIndex = 0;
    int oldestEntryCounter = cache[index][0].counter;
    bool foundExistingEntry = false;
    int existingEntryIndex = -1;
    cout << "MISS!!" << endl;
    for (int i = 0; i < cache[index].size(); i++) {
        if (cache[index][i].counter < oldestEntryCounter) {
            oldestEntryIndex = i;
            oldestEntryCounter = cache[index][i].counter;
        }
        if (cache[index][i].valid && cache[index][i].tag == tag) {
            foundExistingEntry = true;
            existingEntryIndex = i;
        }
    }
    if (foundExistingEntry) {
        cache[index][existingEntryIndex].counter = 0;
        cache[index][existingEntryIndex].address = address;
    } else {
        cache[index][oldestEntryIndex] = { true, tag, 0, address };
    }
    // Increment counters for all cache entries
    for (int i = 0; i < cache[index].size(); i++) {
        cache[index][i].counter++;
    }
    return false;
}


void printCache(vector<vector<CacheEntry>>& cache) {
    cout << "Cache contents:" << endl;
    for (int i = 0; i < cache.size(); i++) {
        for (int j = 0; j < cache[i].size(); j++) {
            cout << "[" << i << "][" << j << "]: ";
            if (cache[i][j].valid) {
                cout << "valid, tag = " << cache[i][j].tag << ", counter = " << cache[i][j].counter << ", address = " << cache[i][j].address << endl;
            } else {
                cout << "invalid" << endl;
            }
        }
    }
    cout << endl;
}



int main(int argc, char** argv) {
    // Parse command line arguments
    int numEntries = stoi(argv[1]);
    int associativity = stoi(argv[2]);
    int numIndexBits = log2(numEntries / associativity);
    int numOffsetBits = log2(4); // Assume 4-byte word
    
    // Initialize cache
vector<vector<CacheEntry>> cache(numEntries / associativity, vector<CacheEntry>(associativity, {false, 0, 0, -1}));
    
    // Open memory reference file
    ifstream fin;
    fin.open(argv[3]);
    if (!fin) {
        cout << "Error opening " << argv[3] << "." << endl;
        return 1;
    }
    
    // Open output file
    ofstream outFile("cache_sim_output.txt");
    if (!outFile) {
        cout << "Error opening output file." << endl;
        return 1;
    }
    
    // Simulate cache accesses and write results to output file
    int address;
    while (fin >> hex >> address) {
        bool hit = accessCache(address, cache, numIndexBits, numOffsetBits);
        outFile << dec << address << " : " << (hit ? "HIT" : "MISS") << endl;

        // Print out the cache contents after each access
        cout << "Cache contents:" << endl;
        for (int i = 0; i < cache.size(); i++) {
            cout << "Set " << i << ": ";
            for (int j = 0; j < cache[i].size(); j++) {
                if (cache[i][j].valid) {
                    cout << hex << cache[i][j].tag << " ";
                } else {
                    cout << "- ";
                }
            }
            cout << endl;
        }
        cout << endl;
    }

    
    // Close files
    fin.close();
    outFile.close();
    
    return 0;
}
