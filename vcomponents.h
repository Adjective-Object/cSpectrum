#ifndef SPECTRUM_EQCOMPONENTS_INCLUDED
#define SPECTRUM_EQCOMPONENTS_INCLUDED
#include "json/json.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "anchor.h"
#include "fftmanager.h"
#include "spectrumutil.h"
#include <stdlib.h>

extern SDL_Rect Spectrum_screenbounds;
extern SDL_Renderer *Spectrum_renderer;
extern float Spectrum_screenratio;
extern bool verbose;

// perhaps bad nomenclature, not all
// elements are "Equalizers" / FFT Visualizers
class EQComponent{
public:
	virtual void renderToSurface(
		SDL_Renderer *renderer, 
		int timeStepMillis) {};
	virtual std::string repr() {return "<EQComponent with unimplented repr>";};
};

/******************************************
 * Begin Definitions of actual Components *
 ******************************************/


/**
 * A generic EQ
 **/
class LinearEq : public EQComponent{
protected:
	Anchor anchor;
	int nBars;
	int length;
	int eqheight;

	Direction direction;

	//cached values for optimization
	std::pair<int, int> offset;

public:
	LinearEq(
		Anchor anchorPt, Direction direction, int numBars);
	virtual void renderToSurface(
		SDL_Renderer *renderer, 
		int timeStepMillis) {};
	virtual std::string repr() {return "<Generic EQ with unimplented repr>";};

};


/**
 * solid color bars
 **/
class SimpleBarEq : public LinearEq{
	
	SDL_Color color;
	SDL_Rect drawrect;
	int barwidth;
	float barpadding;

public:
	SimpleBarEq(
		Anchor anchorPt, Direction direction, int numBars, 
			SDL_Color barColor, int barwidth);
	~SimpleBarEq();

	void renderToSurface(
		SDL_Renderer *renderer, 
		int timeStepMillis);

	std::string repr();
};


/**
 * It's just text
 **/
class TextComponent : public EQComponent {
	Anchor anchor;
	std::string text;
	TTF_Font *font;
	SDL_Color color;

	//cached values for optimization
	SDL_Rect drawrect;
	SDL_Texture *textTexture;

public:
	TextComponent(Anchor a, std::string txt, 
			TTF_Font *dfont, SDL_Color c);
	~TextComponent();

	void renderToSurface(
		SDL_Renderer *renderer, 
		int timeStepMillis);
	
	std::string repr();
};

/**
 * Background Image that scales to fit screen
 **/
class BackgroundImage : public EQComponent{
	SDL_Texture *image;
	std::string path;

public:
	BackgroundImage(std::string imgpath);
	~BackgroundImage();

	void renderToSurface(
		SDL_Renderer *renderer, 
		int timeStepMillis);

	std::string repr();

};


#endif