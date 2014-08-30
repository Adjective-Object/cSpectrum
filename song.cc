#include <cstdio>
#include "song.h"
#include <stdlib.h>
using namespace std;

static int const SPECTRUM_PLAYERBUFFCOMMAND = 100;

// Date constructor
Player::Player(char *filename) {
	//will fail on non-null terminated strings
	char *command = (char *)malloc(sizeof(char) * SPECTRUM_PLAYERBUFFCOMMAND);
	snprintf(
		command, 
		SPECTRUM_PLAYERBUFFCOMMAND, 
		"ffmpeg -i %s -f s16le -acodec pcm_s16le -ar 44100 -ac 2 -");
	//where does stderr even go?
	stream = popen(command, "r");

	framedata = (int *)malloc(sizeof(int) * SPECTRUM_FRAME_SIZE);
	_currentframe = 0;

	_totalframes = 0; //TODO calculate song size
}

//opens the ffmpeg subprocess
void Player::next_frame() {

}




MetaDater::MetaDater(char *filename) {
	char *command = (char *)malloc(sizeof(char) * SPECTRUM_PLAYERBUFFCOMMAND);
	snprintf(
		command, 
		SPECTRUM_PLAYERBUFFCOMMAND, 
		"ffprobe %s");
	//where does stderr go?
	FILE *stream = popen(command, "r");

	char s[100];
	string ref = "Input #0, ";
	int found = 0;

}


