#include "json/json.h"
#include <iostream>
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "spectrumutil.h"
#include "vcomponents.h"
#include "parsejson.h"

using namespace std;

void parseBarDims (SimpleBarDims *dims, Json::Value *def){
	if ( !(*def)["barcount"].isNull() &&
		 !(*def)["barwidth"].isNull()	) {
		dims->barcount = (*def)["barcount"].asInt();
		dims->barwidth = (*def)["barwidth"].asInt();
	}
	else if (
			!(*def)["barcount"].isNull() && 
			!(*def)["barpadding"].isNull()	) {
		dims->barcount = (*def)["barcount"].asInt();
		int bc = (*def)["barcount"].asInt();
		dims->barwidth = 
			((*def)["width"].asInt() - 
			((bc-1) * (*def)["barpadding"].asInt()))
				/ bc;
	} else {
		cerr << "bareq poorly defined "
			 << "(needs barcount/barwidth or "
			 << "barcount/barpadding)";
	}
}


EQComponent *makeSimpleBarEq(Json::Value def){
	SimpleBarDims d;
	parseBarDims(&d, &def);
	return new SimpleBarEq(
		loadAnchor(def),
		decodeDirection(def["direction"].asString()),
		d.barcount, 
		def["reversed"].asBool(),

		decodeColor(def["color"].asString()),
		d.barwidth
	);
}

EQComponent *makeTextComponent(Json::Value def){
	printf("%s %d\n",
		def["font"].asString().c_str(),
		def["fontsize"].asInt());

	return new TextComponent(
		loadAnchor(def),
		def["text"].asString(), 
		TTF_OpenFont(
			(def["font"].asString()).c_str(),
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
		if (cMap.find(compType) == cMap.end()) {
			cout <<
				"error: no entry for %s" <<
				compType;
		} else{
			clist[i] = cMap[compType](components[i]);
		}
	}

	return clist;
}

