#ifndef SPECTRUM_EQCOMPONENTS_INCLUDED
#define SPECTRUM_EQCOMPONENTS_INCLUDED
#include "json/json.h"
#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"

//perhaps bad nomenclature
class EQComponent{
public:
	virtual void renderToSurface(
		SDL_Surface *texture, 
		int timeStepMillis, 
		std::vector<int> *fftbuffer) {};
	virtual std::string repr() {return "<EQComponent with unimplented repr>";};
};

typedef struct Anchor {
	std::pair<float, float> worldAnchor;
	std::pair<float, float> localAnchor;
	std::pair<int, int> offset;
} Anchor;

std::string anchorRepr(Anchor a);
Anchor loadAnchor(Json::Value def);

std::vector<EQComponent *> getComponentVectors(Json::Value components);

//
// The components
//

class SimpleBarEq : public EQComponent{
	Anchor anchor;

	int nBars;
	long color;


public:
	SimpleBarEq(Anchor anchorPt, int numBars, Uint32 barColor);
	void renderToSurface(
		SDL_Surface *texture, 
		int timeStepMillis, 
		std::vector<int> *fftbuffer);

	std::string repr();
};


class TextComponent : public EQComponent {
	Anchor anchor;

	std::string text;
	TTF_Font *font;

public:
	TextComponent(Anchor a, std::string txt, TTF_Font *dfont);
	void renderToSurface(
		SDL_Surface *texture, 
		int timeStepMillis, 
		std::vector<int> *fftbuffer);
	
	std::string repr();
};

class BackgroundImage : public EQComponent{
	SDL_Surface *image;

public:
	BackgroundImage(SDL_Surface *img);
	std::string repr();
	void renderToSurface(
		SDL_Surface *texture, 
		int timeStepMillis, 
		std::vector<int> *fftbuffer);
};


#endif