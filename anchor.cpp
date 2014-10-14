#include <iostream>
#include <string>
#include <cstring>
#include <stdlib.h>     /* stoul */
#include "SDL2/SDL.h"
#include "json/json.h"
#include "anchor.h"

using namespace std;

static bool tMapInited = false;
static map<string, float>tMap;
extern SDL_Rect Spectrum_screenbounds;

void initializeTermMapping(){
	if(!tMapInited){
		tMapInited = true;

		tMap["top"] 	= 0.0f;
		tMap["middle"] 	= 0.5f;
		tMap["bottom"] 	= 1.0f;
		tMap["left"] 	= 0.5f;
		tMap["right"] 	= 1.0f;
	}
}

float pFloat(string s) {
	if (tMap.find(s) == tMap.end()){
		return stof(s);
	} else{
		return tMap[s];
	}
}

pair<float,float> parseFloatPair(Json::Value def) {
	if (!def.isString()){
		cerr << "anchor pair not a string\n";
		cerr << def;
		exit(1);
	}

	//typesafety is 4 nerdz;
	char *s = const_cast<char *>(def.asString().c_str());
	char *spaceindex = const_cast<char *>(strchr(s, ' '));

	if (spaceindex == NULL){
		cerr << "anchor pair has no space\n";
		cerr << def;
		exit(1);
	}

	*(spaceindex) = '\0';
	string first = string(s);
	string second = string(spaceindex+1);

	initializeTermMapping();
	return make_pair<float,float>(
		pFloat(first),
		pFloat(second)
		);
}

pair<int,int> parseIntPair(Json::Value def){
	if (def[0].isNull() || def[1].isNull() ){
		cerr << "int pair does not have children 1 and 2\n";
		cerr << def;
		exit(1);
	}
	if (!def[0].isInt() || !def[1].isInt() ){
		cerr << "attempred to make pair of ints from non ints\n";
		cerr << def;
		exit(1);
	}
	return make_pair<int,int>(def[0].asInt(), def[1].asInt());
}

Anchor loadAnchor(Json::Value def){
	//check all the required contents are there, print out
	//on failure
	string props[3];
	props[0] = "worldanchor";
	props[1] = "localanchor";
	props[2] = "offset";

	for (int i=0; i<3; i++){
		if (def[props[i]].isNull()){
			cerr << props[i] << "missing from definition:\n";
			cerr << def;
			exit(1);
		}		
	}

	return Anchor{
		.worldAnchor = parseFloatPair(def["worldanchor"]),
		.localAnchor = parseFloatPair(def["localanchor"]),
		.offset = parseIntPair(def["offset"]),
	};
}

pair<int, int> getAnchorOffset(Anchor *a){
    return pair<float,float>(
        (int) (Spectrum_screenbounds.w * a->worldAnchor.first) +
        a->offset.first,

        (int)(Spectrum_screenbounds.h * a->worldAnchor.second) +
        a->offset.second
        );
}


//
//	Definitions of component contents
//

string anchorRepr(Anchor a){
	char out[200];
	sprintf(out, 
		"<Anchor world=(%.2f, %.2f) local=(%.2f, %.2f) offset=(%d, %d)>",
		
		a.worldAnchor.first,
		a.worldAnchor.second,
		
		a.localAnchor.first,
		a.localAnchor.first,

		a.offset.first,
		a.offset.first
		);
	return out;
}