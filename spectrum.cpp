#include <iostream>
#include <sys/time.h>
#include <sys/unistd.h>
#include "json/json.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

#include "vcomponents.h"
#include "fftparser.h"
#include "parsejson.h"
#include "song.h"

#include <fstream>

extern "C" {
#include <libavformat/avformat.h>
}

using namespace std;

SDL_Rect Spectrum_screenbounds;
double Spectrum_screenratio;

SDL_Window *gWindow;
SDL_Renderer *Spectrum_renderer;

int xresolution;
int yresolution;

bool preview = false;
bool verbose = false;

char configBuffer[300];
string configDirectory;


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

	configDirectory = string(cfgfilename);
	configDirectory = configDirectory.substr(0, configDirectory.find_last_of("/\\"));
	if (configDirectory.find_last_of("/") == string::npos){
		configDirectory = "./";
	} else{
	}

	if(verbose) {
		cout << " sound: "<< (soundfilename) << endl;
		cout << "config: " << (cfgfilename) << endl;
		cout << "cfgdir: " << (configDirectory) << endl;
	}

	return make_pair(soundfilename, cfgfilename);
}


// this should probably put the tree on the heap so it doesn't need
// to be passed down, buttfuckit.
Json::Value loadJsonCfg(char *cfgfilename){
	//load the config string

    std::ifstream ifs(cfgfilename);
    std::string cfgstring ( (std::istreambuf_iterator<char>(ifs) ),
                            (std::istreambuf_iterator<char>()  ) );

    if (cfgstring == "") {
        cout << "could not open " << cfgfilename << endl;
        exit(1);
    }

	// get the json tree from config string
	Json::Value root;   // will contain the root value after parsing.
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

void makeFakeBuffer(vector<double> *fftbuffer) {
	for(size_t i=0; i<fftbuffer->size(); i++){
		(*fftbuffer)[i] = rand()/ (float)(RAND_MAX);
	}
}

void pushToBuffer(vector<double> & fftbuffer, uint8_t *raw) {
	for (int i=0; i<SPECTRUM_BUFFSIZE; i++) {
        fftbuffer[i] = *raw / 255.0;
		raw++;
	}
}

void mainloop(Json::Value config, Song *song, vector<EQComponent *> components) {

	SDL_Color bkgfill = decodeColor(config["bkgfill"].asString());

	printf("entering mainloop (%d, %d)\n",
			xresolution, yresolution);

	// buffer for the fft output data
    // TODO iniitialize with proper dimensions based on fft
	vector<double> fftbuffer(SPECTRUM_BUFFSIZE);

	struct timeval lastframe, tp;
	gettimeofday(&lastframe, NULL);
	long int dtime = 1000000/60;

	FFTParser * parser = new FFTParser(song->reader->output_buffer);

	SDL_Event e;
	bool quit = false;
	while(!quit){
		while( SDL_PollEvent( &e ) != 0 ) {
			//User requests quit
			if( e.type == SDL_QUIT ) {
				quit = true;
			}
		}

		// advance buffer and get fft data
		song->reader->next_frame();
		//pushToBuffer(fftbuffer, song->reader->output_buffer);
		//makeFakeBuffer(&fftbuffer);
		parser->doAnalysis(fftbuffer);

        int sum = 0;
        for (int i=0; i<SPECTRUM_BUFFSIZE; i++) {
            sum += song->reader->output_buffer[i];
        }

        cout << "AVG VALUE IN BUFFER: "
        << (float)(sum) / SPECTRUM_BUFFSIZE << endl;

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
			usleep((__useconds_t)(lfms - tpms + dtime));
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

    //soundfile, cfgfile
	pair<char *, char *> parsed = parseArgs(argc, argv);

    char cwd[1024];
    getcwd(cwd, 1024);
    if (verbose) cout << "== working directory : " << cwd << endl;

    if (verbose){cout << "== decoding JSON" << endl;}
	Json::Value root = loadJsonCfg(parsed.second);

	//set the cwd to the directory of the config file
	if (verbose) cout << "== changing directory to config dir" <<endl;
	chdir(configDirectory.c_str());
	getcwd(cwd, 1024);
	if (verbose) cout << "== working directory : " << cwd << endl;


    if(verbose) cout << "== initializing ffmpeg" << endl;
    av_register_all();

    if(verbose) cout << "== initializing sdl" << endl;
    //Start SDL
	SDL_Init( SDL_INIT_EVERYTHING );
	TTF_Init();

    if(verbose) cout << "== initializing Song" << endl;

	//make the song
	Song song = Song(parsed.first);

    if(verbose) cout << "== initializing window" << endl;
    initWindow(root["config"]);

    if(verbose) cout << "== decoding components from json" << endl;

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

    if(verbose) cout << "== loading song" << endl;

    //enter mainloop
	mainloop(
		root["config"], 
		&song,
		contents);

	//Quit SDL
	SDL_Quit();
	TTF_Quit();

	return 0;
}
