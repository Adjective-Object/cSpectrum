#include <vector>
#include <map>
#include "fftmanager.h"

using namespace std;

//TODO implement this
vector<float> compressBars(vector<float> *bins, int nbars){
    vector<float> newvector = vector<float>(nbars);
    for (int i=0; i<nbars; i++){
        newvector[i] = 0;
    }
    return newvector;
}

//TODO optimize this so the float vectors are on heap
//so we can get a faster initialization
static vector<float> rootbin;
static map<int, vector<float>> cachedbins = map<int, vector<float>>();
void FFT_setFrameBin(vector<float> fftbin){
    rootbin = fftbin;
    cachedbins.clear();
    cachedbins[fftbin.size()] = fftbin;
}

//returns a new equivalent vector with n bins.
//also caches that bin compression for subsequent calls on the same frame step
vector<float> *FFT_getBins(int nbars){
    try {
        return &cachedbins.at(nbars);
    } catch(const out_of_range& oor){
        cachedbins[nbars] = compressBars(&rootbin, nbars);
        return &cachedbins.at(nbars);
    }
}