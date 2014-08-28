#include <iostream>
#include "json/json.h"
#include "SDL/SDL.h"
#include "vcomponents.h"
#include "song.h"
using namespace std;

void youDidItWrong(){
	 cout << ("args format is spectrum <flags> [soundfile] [cfgfile]\n");
	 cout << ("flags are:\n");
	 cout << ("\t-p: preview in window without rendering video\n");
	 cout << ("\t-v: print verbose output\n");

	 exit(1);
}

int main (int argc, char *argv []) {
	//usage is "spectrum soundfile cfgfile"q
	if (argc < 2) {
		 youDidItWrong();
	}

	int preview = 0;
	int verbose = 0;

	int ccount = 0;
	do{

		ccount = ccount + 1;
		if (argc - ccount < 2){
			 youDidItWrong();
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
		return 1;
	}

	vector<VComponent> contents = getComponentVector(root["components"]);

	//Start SDL
	SDL_Init( SDL_INIT_EVERYTHING );

	//Quit SDL
	SDL_Quit();

	return 0;
}