#include <iostream>
#include <string>
#include <stdlib.h>     /* stoul */
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "json/json.h"
#include "spectrumutil.h"
#include "anchor.h"
#include "vcomponents.h"
using namespace std;


//
// Actual parsing of components
// TODO all verification of anything at all
//

EQComponent *makeSimpleBarEq(Json::Value def){
	return new SimpleBarEq(
		loadAnchor(def),
		def["barcount"].asInt(), 
		(unsigned int) stol(def["color"].asString(), NULL, 16),
		def["height"].asInt(),
		def["barpadding"].asInt(),
		def["barwidth"].asInt()
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



// Simple Bar EQ

SimpleBarEq::SimpleBarEq(
	Anchor anchorPt, int numBars, Uint32 barColor,
	int height, int barpadding, int barwidth){

	//internals
	this->anchor = anchorPt;
	this->nBars = numBars;
	this->color = barColor;
	this->height = height;
	this->barpadding = barpadding;
	this->barwidth = barwidth;

	this->offset = getAnchorOffset(&anchorPt);

	this->drawrect = SDL_Rect{
		.x = 0,
		.y = this->offset.second,
		.w = barwidth,
		.h = 0
	};
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
		SDL_Surface *surface, 
		int timeStepMillis){

	vector<float> *bars = FFT_getBins(this->nBars);

	for(uint i = this->nBars; i>0; i--){
		this->drawrect.x = 
			this->offset.first + 
			i * (this->barwidth + this->barpadding);
		this->drawrect.h = this->height * (*bars)[i];

		SDL_FillRect(surface, &(this->drawrect), this->color);
	}
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
	int timeStepMillis){
	//TODO
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
	int timeStepMillis){

	SDL_BlitSurface(
		this->image, NULL,
		targetSurface, NULL
		);

}
