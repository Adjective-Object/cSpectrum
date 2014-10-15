#ifndef SPECTRUM_ANCHOR_INCLUDED
#define SPECTRUM_ANCHOR_INCLUDED

#include "json/json.h"
#include <stdlib.h>

typedef struct Anchor {
	std::pair<float, float> worldAnchor;
	std::pair<float, float> localAnchor;
	std::pair<int, int> offset;
	int width;
	int height;
} Anchor;

std::string anchorRepr(Anchor a);
Anchor loadAnchor(Json::Value def);

std::pair<int, int> Anchor_GetOffset(Anchor *a);

std::string anchorRepr(Anchor a);

#endif