
spectrum: spectrum.cc song.h song.cc vcomponents.h vcomponents.cc 
	g++ -o spectrum song.cc vcomponents.cc spectrum.cc -lSDL -ljsoncpp