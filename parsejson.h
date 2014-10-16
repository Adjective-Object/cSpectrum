#ifndef SPECTRUM_JSON_PARSER_DEFINED
#define SPECTRUM_JSON_PARSER_DEFINED
#include "json/json.h"
#include "spectrumutil.h"
#include "vcomponents.h"

std::vector<EQComponent *> getComponentVectors(Json::Value components);
#endif