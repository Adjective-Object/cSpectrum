#include <vector>
#include <map>
#include "fftmanager.h"

using namespace std;

static vector<float> rootbin;
static map<int, vector<float>> cachedbins = map<int, vector<float>>();

//TODO implement this
vector<float> compressBars(vector<float> *bins, int nBars){
    vector<float> newvector = vector<float>(nBars);
    
    float step = 1.0f / nBars, sum;
    int rootStop = 0;

    for (int i=0; i<nBars; i++){
        int stop = (step*(i+1)) * bins->size();
        sum = 0;
        for (int x=rootStop; x<stop; x++) {
            sum = sum + (*bins)[x];
        }
        newvector[i] = sum / (float)(stop - rootStop);
        rootStop = stop;

    }
    return newvector;
}

//TODO optimize this so the float vectors are on heap
//so we can get a faster initialization
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