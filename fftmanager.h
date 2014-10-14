#ifndef SPECTRUM_FFTMANAGER_INCLUDED
#define SPECTRUM_FFTMANAGER_INCLUDED

#include <stdlib.h>
#include <vector>

void FFT_setFrameBin(std::vector<float> vectors);
std::vector<float> *FFT_getBins(int nbars);

#endif