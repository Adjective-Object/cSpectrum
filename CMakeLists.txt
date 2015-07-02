cmake_minimum_required(VERSION 3.2)
project(cSpectrum)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "/home/mhuan13/Projects/cSpectrum")

set(SOURCE_FILES
    abspath.c
    abspath.h
    anchor.cpp
    anchor.h
    fftmanager.cpp
    fftmanager.h
    fftparser.cpp
    fftparser.h
    parsejson.cpp
    parsejson.h
    relpath.h
    song.cpp
    song.h
    spectrum.cpp
    spectrumutil.cpp
    spectrumutil.h
    vcomponents.cpp
    vcomponents.h)

add_executable(cSpectrum ${SOURCE_FILES})

target_link_libraries (cSpectrum LINK_PUBLIC jsoncpp)
target_link_libraries (cSpectrum LINK_PUBLIC fftw3)
target_link_libraries (cSpectrum LINK_PUBLIC SDL2)
target_link_libraries (cSpectrum LINK_PUBLIC SDL2_image)
target_link_libraries (cSpectrum LINK_PUBLIC SDL2_mixer)
target_link_libraries (cSpectrum LINK_PUBLIC SDL2_ttf)