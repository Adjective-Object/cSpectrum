#SDL_FLAGS = -ljsoncpp -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf
SDL_FLAGS = -ljsoncpp -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf

spectrum: spectrum.cpp song.o vcomponents.o spectrumutil.o anchor.o fftmanager.o
	g++ -Wall -g -std=c++11 -m64 -o spectrum\
		$^\
		-L/usr/lib64/ $(SDL_FLAGS)

%.o: %.cpp %.h
	g++ -std=c++11 -c $<

clean:
	rm *.o
	rm ./spectrum
