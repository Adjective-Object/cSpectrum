#include "fftparser.h"
#include "song.h"
#include <fftw3.h>
#include <vector>
#include <math.h>
#include <float.h>

using namespace std;

FFTParser::FFTParser(double *source_buffer){

	in = source_buffer;

     // double the size, because converting from real to halfcomplex?
     fftw_out_buffer = (double*) fftw_malloc(sizeof(double) * SPECTRUM_BUFFSIZE * 2);
     plan = fftw_plan_r2r_1d(
     	SPECTRUM_BUFFSIZE, in, fftw_out_buffer, 
     	FFTW_R2HC, 
     	FFTW_MEASURE
     );

}

FFTParser::~FFTParser(){
	fftw_destroy_plan(plan);
	fftw_free(fftw_out_buffer);
}

void FFTParser::doAnalysis(vector<double> & vector_out){
	// execute fftw plan
	fftw_execute(plan);
	double * num = in;

	for (int i=0; i<SPECTRUM_BUFFSIZE; i++) {
		// scale so it's in the 0-1 range
		num[i] = isnan(num[i]) ? 0 : fabs(num[i]);

		if (num[i] < FFT_OUT_MIN)
			num[i] = FFT_OUT_MIN;

		if (num[i] > FFT_OUT_MAX)
			num[i] = FFT_OUT_MAX;

		// copy to vector
		vector_out[i] = (num[i] - FFT_OUT_MIN) / (FFT_OUT_MAX - FFT_OUT_MIN);
	}
}
