#ifndef SPECTRUM_SDLHELPER_INCLUDED
#define SPECTRUM_SDLHELPER_INCLUDED
#include <iostream>
#include "SDL2/SDL.h"
#include "anchor.h"
#include "json/json.h"

SDL_Surface *ScaledCroppedCopy(
    SDL_Surface *source, 
    SDL_Rect destDim);

SDL_Color decodeColor(std::string color);

typedef enum Direction {
	North,
	South,
	East,
	West
} Direction;

Direction decodeDirection(std::string value);

typedef struct EQProps{
	Anchor anchor;
	Direction direction;
	int length;
	int barsize;
	int barcount;
} EQProps;

#endif
