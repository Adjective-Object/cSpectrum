#ifndef SPECTRUM_EQCOMPONENTS_INCLUDED
#define SPECTRUM_EQCOMPONENTS_INCLUDED
#include "json/json.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "anchor.h"
#include "fftmanager.h"
#include <stdlib.h>

extern SDL_Rect Spectrum_screenbounds;
extern SDL_Renderer *Spectrum_renderer;
extern float Spectrum_screenratio;
extern bool verbose;


//perhaps bad nomenclature
class EQComponent{
public:
	virtual void renderToSurface(
		SDL_Renderer *renderer, 
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
	Uint8 alpha;
	SDL_Color color;

	int barwidth, barpadding;

	bool direction;

	//cached values for optimization
	std::pair<int, int> offset;
	SDL_Rect drawrect;

public:
	SimpleBarEq(Anchor anchorPt, int numBars, SDL_Color color,
		int barpadding, int barwidth, bool direction);
	void renderToSurface(
		SDL_Renderer *renderer, 
		int timeStepMillis);

	std::string repr();
};


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

	void renderToSurface(
		SDL_Renderer *renderer, 
		int timeStepMillis);
	
	std::string repr();
};

class BackgroundImage : public EQComponent{
	SDL_Texture *image;
	std::string path;

public:
	BackgroundImage(std::string imgpath);
	~BackgroundImage();
	std::string repr();
	void renderToSurface(
		SDL_Renderer *renderer, 
		int timeStepMillis);
};


#endif