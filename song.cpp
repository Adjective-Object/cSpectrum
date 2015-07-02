#include <cstdio>
#include "song.h"
#include <stdlib.h>
#include <vector>
#include <stdio.h>
#include <math.h>
using namespace std;



// Date constructor
SongReader::SongReader(char *path) {
	// intialize counter variables
	_currentframe = 0;
	_totalframes = 0; //TODO calculate song size

    buffer = (double *) malloc(sizeof(double) * SPECTRUM_BUFFSIZE);
}

SongReader::~SongReader(){
	free(buffer);
}

void SongReader::next_frame() {
	// fill the song data buffer with fake data
	double trace = 0;
	for(uint i = 0; i < SPECTRUM_BUFFSIZE; i++) {
		trace = trace / 2 + fabs((double) rand() / RAND_MAX / 60);
		buffer[i] = trace;
	}
}
	




MetaData::MetaData(char *path) {

}


