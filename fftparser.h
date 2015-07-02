#ifndef SPECTRUM_FFTBUFFER_DEFINED
#define SPECTRUM_FFTBUFFER_DEFINED

#define FFT_OUT_MAX 0.02
#define FFT_OUT_MIN 0.01

#include <fftw3.h>
#include <vector>

class FFTParser {

	double *fftw_out_buffer;
	fftw_plan plan;

public:
	
	FFTParser(double *instream);
	~FFTParser();

	double *in;

	void doAnalysis(std::vector<double> & vector_out);
};

#endif
