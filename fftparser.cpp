#include "fftparser.h"
#include "song.h"
#include <fftw3.h>
#include <vector>
#include <math.h>
#include <float.h>

using namespace std;

FFTParser::FFTParser(uint8_t *source_buffer){

	in = source_buffer;
	in_temp_buffer = static_cast<double *>(
            malloc(sizeof(double) * SPECTRUM_BUFFSIZE));

     // double the size, because converting from real to halfcomplex?
     fftw_out_buffer =
             (double*) fftw_malloc(sizeof(double) * SPECTRUM_BUFFSIZE * 2);
     plan = fftw_plan_r2r_1d(
     	SPECTRUM_BUFFSIZE, in_temp_buffer, fftw_out_buffer,
     	FFTW_R2HC,
     	FFTW_MEASURE
     );

}

FFTParser::~FFTParser(){
	fftw_destroy_plan(plan);
	fftw_free(fftw_out_buffer);
}

void FFTParser::doAnalysis(vector<double> & vector_out){
    // convert input into doubles

    for(int i=0; i<SPECTRUM_BUFFSIZE; i++) {
        this->in_temp_buffer[i] = in[i] / 255.0;
    }

	// execute fftw plan
	fftw_execute(plan);
	double * num = in_temp_buffer;

	for (int i=0; i<SPECTRUM_BUFFSIZE; i++) {
		// scale so it's in the 0-1 range
		double d = num[i];
		num[i] = isnan(d) ? 0 : fabs(d);

		if (num[i] < FFT_OUT_MIN)
			num[i] = FFT_OUT_MIN;

		if (num[i] > FFT_OUT_MAX)
			num[i] = FFT_OUT_MAX;

		// copy to vector
		vector_out[i] = (num[i] - FFT_OUT_MIN) / (FFT_OUT_MAX - FFT_OUT_MIN);
	}
}
