#include <iostream>
#include <sys/time.h>
#include <sys/unistd.h>
#include <stdlib.h>
#include "json/json.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_image.h"
#include "song.h"
#include "vcomponents.h"
#include "spectrumutil.h"
#include "fftmanager.h"

using namespace std;

SDL_Rect Spectrum_screenbounds;
float Spectrum_screenratio;

SDL_Window *gWindow;
SDL_Renderer *Spectrum_renderer;

int xresolution;
int yresolution;

bool preview = 0;
bool verbose = 0;

void printHelpMessage(){
	 cout << ("args format is spectrum <flags> [soundfile] [cfgfile]\n");
	 cout << ("flags are:\n");
	 cout << ("\t-p: preview in window without rendering video\n");
	 cout << ("\t-v: print verbose output\n");

	 exit(1);
}

pair<char *, char *> parseArgs(int argc, char *argv[]){
	//usage is "spectrum soundfile cfgfile"q
	if (argc < 2) {
		 printHelpMessage();
	}

	int ccount = 0;
	do{

		ccount = ccount + 1;
		if (argc - ccount < 2){
			 printHelpMessage();
		}

		if (strcmp(argv[0], "-p") == 0){
			preview = 1;
		}else if (strcmp(argv[0], "-v") == 0){
			verbose = 1;
		}

		argv++;

	} while (argv[0][0]=='-') ;

	char *soundfilename = argv[0];
	char *cfgfilename = argv[1];

	if(verbose) {
		cout << "sound: "<< (soundfilename) << endl;
		cout << "config: " << (cfgfilename) << endl;
	}

	return make_pair(soundfilename, cfgfilename);
}

// this should probably put the tree on the heap so it doesn't need
// to be passed down, buttfuckit.
Json::Value loadJsonCfg(char *cfgfilename){
	//load the config string
	
	char cfgstring[8192];
	FILE *fp = fopen(cfgfilename, "r");
	int tcount = 0;
	int count;
	while(0 < (count = fread(cfgstring+tcount, sizeof(char), 1024, fp))){
		tcount = tcount + count;
	}
	cfgstring[tcount+1] = '\0';

	// get the json tree from configstring

	Json::Value root;   // will contains the root value after parsing.
	Json::Reader reader;
	bool parsingSuccessful = reader.parse( cfgstring, root );
	if ( !parsingSuccessful ) {
		// report to the user the failure and their locations in the document.
		cout  << "Failed to parse configuration\n"
				   << reader.getFormattedErrorMessages();
		exit(1);
	}

	return root;
}

void makeFakeBuffer(vector<float> *fftbuffer) {
	for(uint i=0; i<fftbuffer->size(); i++){
		(*fftbuffer)[i] = rand()/ (float)(RAND_MAX);
	}
}

void mainloop(Json::Value config, vector<EQComponent *> components) {

	SDL_Color bkgfill = decodeColor(config["bkgfill"].asString());

	printf("entering main loop (%d, %d)\n",
			xresolution, yresolution);

	//fftbuffer
	vector<float> fftbuffer(100);

	struct timeval lastframe, tp;
	gettimeofday(&lastframe, NULL);
	long int dtime = 1000000/60;

	SDL_Event e;
	bool quit = false;
	while(!quit){
		while( SDL_PollEvent( &e ) != 0 ) {
			//User requests quit
			if( e.type == SDL_QUIT ) {
				quit = true;
			}
		}

		makeFakeBuffer(&fftbuffer);

		SDL_SetRenderDrawColor(
			Spectrum_renderer,
			bkgfill.r,
			bkgfill.g,
			bkgfill.b,
			bkgfill.a);

		SDL_RenderClear(Spectrum_renderer);
		FFT_setFrameBin(fftbuffer);

		for (uint i=0; i<components.size(); i++){
			components[i]->renderToSurface(
				Spectrum_renderer,
				100);
		}

		//sleep time
		gettimeofday(&tp, NULL); 
		long int tpms = tp.tv_sec * 1000000 + tp.tv_usec;
		long int lfms = lastframe.tv_sec * 1000000 + lastframe.tv_usec;
		if(lfms - tpms + dtime > 0){
			usleep(lfms - tpms + dtime);
		}
		//printf("%ld\n", lfms - tpms + dtime);
		lastframe = tp;

		SDL_RenderPresent(Spectrum_renderer);

	}
}

void initWindow(Json::Value config){
	xresolution = config["resolution"][0].asInt();
	yresolution = config["resolution"][1].asInt();

	Spectrum_screenbounds = SDL_Rect{
		.x = 0,
		.y = 0,
		.w = xresolution,
		.h = yresolution,
	};

	Spectrum_screenratio = (float)(xresolution) / yresolution;

	gWindow = SDL_CreateWindow(
		"cSpectrum", 
		SDL_WINDOWPOS_UNDEFINED, 
		SDL_WINDOWPOS_UNDEFINED, 
		xresolution, 
		yresolution, 
		SDL_WINDOW_SHOWN );
    Spectrum_renderer = SDL_CreateRenderer( gWindow, -1, 0);
    SDL_SetRenderDrawBlendMode(Spectrum_renderer,
                               SDL_BLENDMODE_BLEND);
}

int main (int argc, char *argv []) {

	if (verbose){cout << "decoding JSON" << endl;}

	//soundfile, cfgfile
	pair<char *, char *> parsed = parseArgs(argc, argv);
	Json::Value root = loadJsonCfg(parsed.second);

	//Start SDL
	SDL_Init( SDL_INIT_EVERYTHING );
	TTF_Init();

	initWindow(root["config"]);

	//decode components from the JSON
	vector<EQComponent *> contents = getComponentVectors(
		root["components"]
	);
	if(verbose){
		for (uint i=0; i<contents.size(); i++){
			cout << endl << contents[i]->repr() <<endl;
		}
		cout << endl;
	}

	//enter mainloop
	mainloop(root["config"], contents);

	//Quit SDL
	SDL_Quit();
	TTF_Quit();

	return 0;
}