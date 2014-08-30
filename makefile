
spectrum: spectrum.cc song.h song.cc vcomponents.h vcomponents.cc 
	g++ -std=c++11 -o spectrum song.cc vcomponents.cc spectrum.cc -ljsoncpp  -lSDL -lSDL_ttf