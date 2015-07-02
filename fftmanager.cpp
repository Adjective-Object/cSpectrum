#include <vector>
#include <map>
#include "fftmanager.h"

using namespace std;

static vector<double> rootbin;
static map<int, vector<double>> cachedbins = map<int, vector<double>>();

//TODO implement this
vector<double> compressBars(vector<double> *bins, int nBars){
    vector<double> newvector = vector<double>(nBars);
    
    double step = 1.0 / nBars, sum;
    int rootStop = 0;

    for (int i=0; i<nBars; i++){
        int stop = (step*(i+1)) * bins->size();
        sum = 0;
        for (int x=rootStop; x<stop; x++) {
            sum = sum + (*bins)[x];
        }
        newvector[i] = sum / (double)(stop - rootStop);
        rootStop = stop;

    }
    return newvector;
}

//TODO optimize this so the double vectors are on heap
//so we can get a faster initialization
void FFT_setFrameBin(vector<double> fftbin){
    rootbin = fftbin;
    cachedbins.clear();
    cachedbins[fftbin.size()] = fftbin;
}

//returns a new equivalent vector with n bins.
//also caches that bin compression for subsequent calls on the same frame step
vector<double> *FFT_getBins(int nbars){
    try {
        return &cachedbins.at(nbars);
    } catch(const out_of_range& oor){
        cachedbins[nbars] = compressBars(&rootbin, nbars);
        return &cachedbins.at(nbars);
    }
}
