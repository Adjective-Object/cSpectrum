#ifndef SPECTRUM_EQCOMPONENTS_INCLUDED
#define SPECTRUM_EQCOMPONENTS_INCLUDED
#include "json/json.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "anchor.h"
#include "fftmanager.h"
#include <stdlib.h>

extern SDL_Rect Spectrum_screenbounds;
extern SDL_PixelFormat *Spectrum_screenformat;
extern float Spectrum_screenratio;
extern bool verbose;


//perhaps bad nomenclature
class EQComponent{
public:
	virtual void renderToSurface(
		SDL_Surface *targetSurface, 
		int timeStepMillis) {};
	virtual std::string repr() {return "<EQComponent with unimplented repr>";};
};


std::vector<EQComponent *> getComponentVectors(Json::Value components);

//
// The components
//

class SimpleBarEq : public EQComponent{
	Anchor anchor;

	int nBars;
	long color;

	int barwidth, barpadding;

	bool direction;

	//cached values for optimization
	std::pair<int, int> offset;
	SDL_Rect drawrect;

public:
	SimpleBarEq(Anchor anchorPt, int numBars, Uint32 barColor,
		int barpadding, int barwidth, bool direction);
	void renderToSurface(
		SDL_Surface *targetSurface, 
		int timeStepMillis);

	std::string repr();
};


class TextComponent : public EQComponent {
	Anchor anchor;
	std::string text;
	TTF_Font *font;

public:
	TextComponent(Anchor a, std::string txt, TTF_Font *dfont);
	void renderToSurface(
		SDL_Surface *targetSurface, 
		int timeStepMillis);
	
	std::string repr();
};

class BackgroundImage : public EQComponent{
	SDL_Surface *image;
	std::string path;

public:
	BackgroundImage(std::string imgpath);
	~BackgroundImage();
	std::string repr();
	void renderToSurface(
		SDL_Surface *targetSurface, 
		int timeStepMillis);
};


#endif