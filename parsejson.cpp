#include "json/json.h"
#include <iostream>
#include <string>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "json/json.h"
#include "spectrumutil.h"
#include "anchor.h"
#include "vcomponents.h"
#include "parsejson.h"

using namespace std;

//
// Actual parsing of components
// TODO all verification of anything at all
//

EQComponent *makeSimpleBarEq(Json::Value def){
	return new SimpleBarEq(
		loadAnchor(def),
		decodeDirection(def["direction"].asString()),
		def["barcount"].asInt(), 
		def["reversed"].asBool(),

		decodeColor(def["color"].asString()),
		def["barwidth"].asInt()
	);
}

EQComponent *makeTextComponent(Json::Value def){
	printf("%s %d\n",
		("./assets/" + def["font"].asString() + ".ttf").c_str(),
		def["fontsize"].asInt());

	return new TextComponent(
		loadAnchor(def),
		def["text"].asString(), 
		TTF_OpenFont(
			("./assets/" + def["font"].asString() + ".ttf").c_str(),
			def["fontsize"].asInt()
		),
		decodeColor(def["color"].asString())
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

