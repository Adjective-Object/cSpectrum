#ifndef SPECTRUM_FFTBUFFER_DEFINED
#define SPECTRUM_FFTBUFFER_DEFINED

#define FFT_OUT_MAX 0.02
#define FFT_OUT_MIN 0.01

#include <fftw3.h>
#include <vector>
#include <stdint.h>

class FFTParser {

	double *in_temp_buffer;
	double *fftw_out_buffer;
	fftw_plan plan;

public:
	
	FFTParser(uint8_t *instream);
	~FFTParser();

	uint8_t *in;

	void doAnalysis(std::vector<double> & vector_out);
};

#endif
