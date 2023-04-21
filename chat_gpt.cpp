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
    int offset = address & ((1 << numOffsetBits) - 1);

    // Search for a matching tag in the cache
    for (int i = 0; i < cache[index].size(); i++) {
        if (cache[index][i].valid && cache[index][i].tag == tag) {
            // Hit!
            cout << "HIT!!" << endl;
            if (cache[index][i].counter == cache[index].size() - 1) {
                // This entry was already the most recently used, so no need to update the counters
                return true;
            } else {
                // Update the counters for all entries with a higher counter value
                int currentCounter = cache[index][i].counter;
                for (int j = 0; j < cache[index].size(); j++) {
                    if (cache[index][j].counter > currentCounter) {
                        cache[index][j].counter--;
                    }
                }
                // Set the counter of the hit entry to the maximum value
                cache[index][i].counter = cache[index].size() - 1;
                return true;
            }
        }
    }

    // Miss - evict the oldest entry and insert the new one
    cout << "MISS!!" << endl;
    int oldestEntryIndex = 0;
    int oldestEntryCounter = cache[index][0].counter;
    for (int i = 1; i < cache[index].size(); i++) {
        if (cache[index][i].counter < oldestEntryCounter) {
            oldestEntryIndex = i;
            oldestEntryCounter = cache[index][i].counter;
        }
    }
    cache[index][oldestEntryIndex] = { true, tag, static_cast<int>(cache[index].size() - 1) };
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
        printCache(cache);
        cout << endl;
    }

    
    // Close files
    fin.close();
    outFile.close();
    
    return 0;
}
