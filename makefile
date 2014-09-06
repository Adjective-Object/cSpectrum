#SDL_FLAGS = -ljsoncpp -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf
SDL_FLAGS = -ljsoncpp -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf
LD_LIBRARY_PATH = 

spectrum: song.o vcomponents.o spectrum.cc
	g++ -Wall -g -std=c++11 -m64 -o spectrum spectrum.cc song.o vcomponents.o -L/usr/lib64/ $(SDL_FLAGS)

%.o: %.cc %.h
	g++ -std=c++11 -c $<

clean:
	rm *.o
	rm ./spectrum
