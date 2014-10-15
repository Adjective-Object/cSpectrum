#include "SDL2/SDL.h"
#include <iostream>
#include <string>
#include "spectrumutil.h"

using namespace std;
extern SDL_Rect Spectrum_screenbounds;

SDL_Surface *ScaledCroppedCopy(
    SDL_Surface *source, 
    SDL_Rect destDim) {

    float srcRatio = (float)(source->w) / (float)(source->h);
    float destRatio = (float)(destDim.w) / (float)(destDim.h);

    cout << "source ratio: " << srcRatio  << endl;
    cout << "dest   ratio: " << destRatio << endl;

    SDL_Rect srcRect;

    if(destRatio > srcRatio){
        //destination is wider ratio than child
        //so fit child to width
        int cropheight = ((1/destRatio) * source->w);
        cout << "fit height " << cropheight <<endl;

        srcRect = SDL_Rect{
            .x = 0,
            .y = (cropheight -  source->w)/2,
            .w = source->w, 
            .h = cropheight};
    } else{
        //destination is taller ratio than child
        //so fit child to height
        int cropwidth = (int)(destRatio * source->h);
        cout << "fit width " << cropwidth << endl;

        srcRect = SDL_Rect{
            .x = (cropwidth - source->h)/2,
            .y = 0,
            .w = cropwidth,
            .h = source->h};
    }


    SDL_Surface *destSurf = SDL_CreateRGBSurface(
        0,destDim.w,destDim.h,32,0,0,0,0);

    SDL_BlitScaled(source, &srcRect, destSurf, &destDim);
    return destSurf;
}

SDL_Color decodeColor(string strcolor){
    Uint32 bitcolor = (unsigned int) stol(strcolor, NULL, 16);
    return SDL_Color{
        .r = (Uint8)( (bitcolor & 0xff000000) / 0x1000000 ),
        .g = (Uint8)( (bitcolor & 0x00ff0000) / 0x0010000 ),
        .b = (Uint8)( (bitcolor & 0x0000ff00) / 0x0000100 ),
        .a = (Uint8)( bitcolor & 0x000000ff )
    };
}
