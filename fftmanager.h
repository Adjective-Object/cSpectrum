#ifndef SPECTRUM_FFTMANAGER_INCLUDED
#define SPECTRUM_FFTMANAGER_INCLUDED

#include <stdlib.h>
#include <vector>

void FFT_setFrameBin(std::vector<double> vectors);
std::vector<double> *FFT_getBins(int nbars);

#endif
