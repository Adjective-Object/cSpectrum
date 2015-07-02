#SDL_FLAGS = -ljsoncpp -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf
SDL_FLAGS = -ljsoncpp -lfftw3 -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf
.PHONY: test grind

all: cSpectrum

cSpectrum:	spectrum.cpp song.o vcomponents.o \
			spectrumutil.o anchor.o fftmanager.o parsejson.o \
			fftparser.o

	g++ -Wall -g -std=c++11 -m64 -o spectrum\
		$^\
		-L/usr/lib64/ $(SDL_FLAGS)

%.o: %.cpp %.h
	g++ -std=c++11 -c $<

clean:
	rm -f *.o
	rm -f ./spectrum

test: cSpectrum
	./cSpectrum -p assets/haze.mp3 test.json

grind:
	-valgrind --track-origins=yes \
		--read-var-info=yes \
		--suppressions=suppressions.supp \
		./cSpectrum -p assets/haze.mp3 test.json 2> val
	cat val
