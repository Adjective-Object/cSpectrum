#ifndef SPECTRUM_SDLHELPER_INCLUDED
#define SPECTRUM_SDLHELPER_INCLUDED
#include <iostream>
#include "SDL2/SDL.h"
#include "anchor.h"

SDL_Surface *ScaledCroppedCopy(
    SDL_Surface *source, 
    SDL_Rect destDim);

#endif