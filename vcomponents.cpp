#include "json/json.h"
#include "vcomponents.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "sdlhelper.h"
#include <iostream>
#include <string>
#include <stdlib.h>     /* strtoul */
using namespace std;

//
// for parsing anchors
//

// mapping of terms to floats

static bool tMapInited = false;
static map<string, float>tMap;

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

float pFloat(string s){
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


//
// Actual parsing of components
// TODO all verification of anything at all
//

EQComponent *makeSimpleBarEq(Json::Value def){
	return new SimpleBarEq(
		loadAnchor(def),
		def["barcount"].asInt(), 
		(unsigned int) stol(def["color"].asString(), NULL, 16)
	);
}

EQComponent *makeTextComponent(Json::Value def){
	return new TextComponent(
		loadAnchor(def),
		def["text"].asString(), 
		TTF_OpenFont(
			 //TODO use sysfonts dir 
			("./assets/" + def["font"].asString() + ".ttf").c_str(),
			def["fontsize"].asInt()
		)
	);
}

EQComponent *makeBackgroundImage(Json::Value def){
	return new BackgroundImage(def["img"].asString());
}

//
// Mapping of component names to the parsing functions
//

static bool cMapInited = false;
static map<string, EQComponent *(*)(Json::Value)>cMap;

void initializeComponentMapping(){
	if(!cMapInited){
		cMapInited = true;

		cMap["bareq"] = makeSimpleBarEq;
		cMap["text"] = makeTextComponent;
		cMap["bkgimg"] = makeBackgroundImage;
	}
}

vector<EQComponent *> getComponentVectors(Json::Value components) {
	initializeComponentMapping();

	vector<EQComponent *> clist = vector<EQComponent *>(components.size());

	for (int i=0; i<components.size(); i++){
		string compType = components[i]["type"].asString();
		if (cMap.find(compType) == cMap.end()){
			cout <<
				"error: no entry for %s" <<
				compType;
		} else{
			clist[i] = cMap[compType](components[i]);
		}
	}

	return clist;
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

// Simple Bar EQ

SimpleBarEq::SimpleBarEq(Anchor anchorPt, int numBars, Uint32 barColor){
	anchor = anchorPt;
	nBars = numBars;
	color = barColor;
}

string SimpleBarEq::repr() {
	char out[100];
	sprintf(out,"<SimpleBarEq nBars=%d color=%x>\n    %s",
	 	nBars,
	 	color,
		anchorRepr(anchor).c_str()
	);
	return out;
}

void SimpleBarEq::renderToSurface(
		SDL_Surface *texture, 
		int timeStepMillis, 
		std::vector<int> *fftbuffer){

}


// Text Components

TextComponent::TextComponent(Anchor a, string txt, TTF_Font *dfont){
	anchor = a;
	text = txt;
	font = dfont;
}

string TextComponent::repr() {
	char out[200];
		sprintf(
			out,
			"<TextComponnt content=\"%s\" font=%s>\n    %s", 
			text.c_str(), 
			(font != NULL) ? 
				TTF_FontFaceFamilyName(font) : "NULL",
			anchorRepr(anchor).c_str()
		);

	return out;
}

void TextComponent::renderToSurface(
		SDL_Surface *texture, 
	int timeStepMillis, 
	std::vector<int> *fftbuffer){

}


 


  
BackgroundImage::BackgroundImage(string imgpath){
	//load,scale, and optimize
	SDL_Surface *loadedSurface;
	SDL_Surface *scaledSurface;
	float loadedRatio;

	if (verbose)
		cout << "initializing background image "<< imgpath << endl;

	loadedSurface = IMG_Load( imgpath.c_str() );

	if (verbose) 
		cout << "scaling/cropping surface" << endl;

	scaledSurface = ScaledCroppedCopy(
		loadedSurface,
		Spectrum_screenbounds);

	if (verbose) 
		cout << "converting surface"  << endl;

	this->image = SDL_ConvertSurface(
		scaledSurface, 
		Spectrum_screenformat,
		0);

	if (verbose)
		cout << "freeing temp surface" <<endl;

	SDL_FreeSurface(loadedSurface);
}

BackgroundImage::~BackgroundImage(){
	SDL_FreeSurface( this->image );
}

string BackgroundImage::repr(){
	return "<Background Image>";
}

void BackgroundImage::renderToSurface(
	SDL_Surface *targetSurface, 
	int timeStepMillis, 
	std::vector<int> *fftbuffer){

	SDL_BlitSurface(
		this->image, NULL,
		targetSurface, NULL
		);

}
