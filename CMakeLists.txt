cmake_minimum_required(VERSION 3.2)
project(cSpectrum)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "/home/mhuan13/Projects/cSpectrum")
set(RUNTIME_OUTPUT_DIRECTORY "/home/mhuan13/Projects/cSpectrum")

# add ffmpeg to search directory (not portable!)

set(CMAKE_LIBRARY_PATH
    ${CMAKE_LIBRARY_PATH}
    /usr/include/ffmpeg/lib/)

set(CMAKE_INSTALL_RPATH "/usr/include/ffmpeg/lib/")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

find_library(libavcodec FFMPEG_FOUND)
message("${FFMPEG_FOUND}")

set(SOURCE_FILES
    abspath.c
    abspath.c
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
#set_target_properties(cSpectrum
#    PROPERTIES COMPILE_FLAGS
#    "-I/usr/include/ffmpeg")

target_include_directories(cSpectrum PRIVATE /usr/include/ffmpeg)

target_link_libraries (cSpectrum LINK_PUBLIC jsoncpp)
target_link_libraries (cSpectrum LINK_PUBLIC fftw3)
target_link_libraries (cSpectrum LINK_PUBLIC avformat)
target_link_libraries (cSpectrum LINK_PUBLIC avcodec)
target_link_libraries (cSpectrum LINK_PUBLIC avutil)
target_link_libraries (cSpectrum LINK_PUBLIC fftw3)
target_link_libraries (cSpectrum LINK_PUBLIC swscale)
target_link_libraries (cSpectrum LINK_PUBLIC SDL2)
target_link_libraries (cSpectrum LINK_PUBLIC SDL2_image)
target_link_libraries (cSpectrum LINK_PUBLIC SDL2_mixer)
target_link_libraries (cSpectrum LINK_PUBLIC SDL2_ttf)


#add_executable(audiotest audiotest.cpp)
#set_target_properties(audiotest
#    PROPERTIES COMPILE_FLAGS
#    "-I/usr/include/ffmpeg")
#target_link_libraries (audiotest LINK_PUBLIC avformat)
#target_link_libraries (audiotest LINK_PUBLIC avcodec)
#target_link_libraries (audiotest LINK_PUBLIC avutil)
