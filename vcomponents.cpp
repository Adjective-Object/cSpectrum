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

// Linear Eq
//TODO easing
LinearEq::LinearEq(
	Anchor anchorPt, Direction direction, int numBars, 
	bool reversed) {

	this->reversed = reversed;
	this->anchor 	= anchorPt;
	this->direction = direction;
	this->nBars 	= numBars;

	if (direction == North || direction == South) {
		this->eqheight 	= anchorPt.height;
		this->length 	= anchorPt.width;
	}
	else {
		this->eqheight 	= anchorPt.width;
		this->length 	= anchorPt.height;
	}

}

// Simple Bar EQ

SimpleBarEq::SimpleBarEq(
		Anchor anchor, Direction direction, int numBars, bool reversed,
			SDL_Color barColor, int barwidth)
	: LinearEq(anchor, direction, numBars, reversed) {
	
	//set internals
	this->color = barColor;
	this->barwidth = barwidth;


	//cache values for optimization
	this->offset = Anchor_GetOffset(&anchor);

	if(direction == North || direction == South){
		this->drawrect = SDL_Rect{
			.x = 0,
			.y = this->offset.second,
			.w = barwidth,
			.h = 0
		};

		//calculate padding based on barwidth and width
		this->barpadding = (float)
								(anchor.width - (barwidth * numBars))
								/ (numBars-1);
	} else{
		this->drawrect = SDL_Rect{
			.x = this->offset.first,
			.y = 0,
			.w = 0,
			.h = barwidth
		};

		//calculate padding based on barwidth and height
		this->barpadding = (float)
								(anchor.height - (barwidth * numBars))
								/ (numBars-1);
	}

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
		SDL_Renderer *renderer, 
		int timeStepMillis){

	vector<float> *bars = FFT_getBins(this->nBars);

	/*
	for (auto c : *bars)
		cout << c << ' ';
	cout << endl;
	*/

	for(uint i = 0; i<this->nBars; i++){
		
		int reg = (this->reversed ? this->nBars - 1 - i : i);
		if(this->direction == South || this->direction == North) {
			this->drawrect.x = 
				this->offset.first + 
				(int)(reg * (this->barwidth + this->barpadding));
		} else{
			this->drawrect.y = 
				this->offset.second + 
				(int)(reg * (this->barwidth + this->barpadding));
		}

		if(this->direction == South){
			//downward
			this->drawrect.h = anchor.height * (*bars)[i];
		} else if (this->direction == North){
			//upward
			this->drawrect.h = anchor.height * (*bars)[i];
			this->drawrect.y = 
				this->offset.second +
				this->anchor.height -
				this->drawrect.h;
		} else if (this->direction == East) {
			//to the right
			this->drawrect.w = anchor.width * (*bars)[i];
		} else if (this->direction == West) {
			//to the left
			this->drawrect.w = anchor.width * (*bars)[i];
			this->drawrect.x = 
				this->offset.first +
				this->anchor.width -
				this->drawrect.w;

		}

		SDL_SetRenderDrawColor(renderer, 
			this->color.r,
			this->color.g,
			this->color.b,
			this->color.a);
		SDL_RenderFillRect(renderer, &(this->drawrect));
	}
}


// Text Components

TextComponent::TextComponent(
	Anchor a, string txt,TTF_Font *dfont, SDL_Color c){

	this->text = txt;
	this->font = dfont;
	this->color = c;

	TTF_SizeText(dfont, txt.c_str(), &(a.width), &(a.height));

	this->anchor = a;

	pair<int, int> offset = Anchor_GetOffset(&a);
	this->drawrect = SDL_Rect {
		.x = offset.first,
		.y = offset.second,
		.w = a.width,
		.h = a.height
	};

	printf("drawrect: %d %d %d %d\n", 
		this->drawrect.x,
		this->drawrect.y,
		this->drawrect.w,
		this->drawrect.h);

	SDL_Surface *s = TTF_RenderText_Blended(
						dfont, txt.c_str(),
						this->color);

	this->textTexture = SDL_CreateTextureFromSurface(
						Spectrum_renderer, 
						s);

	SDL_SetTextureAlphaMod(this->textTexture, c.a);

	SDL_FreeSurface(s);
}

string TextComponent::repr() {
	char out[200];
		sprintf(
			out,
			"<TextComponnt content=\"%s\" font=%s>\n    %s", 
			text.c_str(), 
			(font != NULL) ? 
				TTF_FontFaceFamilyName(font) : "NULL",
			anchorRepr(anchor).c_str());

	return out;
}

void TextComponent::renderToSurface(
		SDL_Renderer *renderer, 
	int timeStepMillis){

	SDL_RenderCopy(
		renderer, this->textTexture, 
		NULL, &(this->drawrect));
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

	this->image = SDL_CreateTextureFromSurface(
		Spectrum_renderer,
		scaledSurface);

	if (verbose)
		cout << "freeing temp surface" <<endl;

	SDL_FreeSurface(scaledSurface);
	SDL_FreeSurface(loadedSurface);
}

BackgroundImage::~BackgroundImage(){
	SDL_DestroyTexture( this->image );
}

string BackgroundImage::repr(){
	return "<Background Image>";
}

void BackgroundImage::renderToSurface(
	SDL_Renderer *renderer, 
	int timeStepMillis){

	SDL_RenderCopy(
		renderer,
		this->image, NULL, NULL
		);
}
