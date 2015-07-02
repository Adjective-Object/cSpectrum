#ifndef SPECTRUM_SONG_INCLUDED
#define SPECTRUM_SONG_INCLUDED

#include <iostream>
#include <stdlib.h>
#include <cstdio>
#include <sys/unistd.h>
#include <vector>


static int const SPECTRUM_PLAYERBUFFCOMMAND = 100;
static int const SPECTRUM_BUFFSIZE = 1024;

// Handles all class metadata. Just a bunch of fancy getters and setters
// All of the string data is on the heap
class MetaData{

    char *_title;
    char *_artist;
    char *_album;
    char *_album_artist;
    char *_composer;

    int _year;
    int _trackno;
    int _numtracks;

    char *_genre;
    char *_originalartist;

public:

    MetaData(char *filename);
    ~MetaData();

    //song metadata
    char *title()           {return _title;}
    char *artist()          {return _artist;}
    char *album()           {return _album;}
    char *album_artist()    {return _album_artist;}
    char *composer()        {return _composer;}

    int year()              {return _year;}
    int trackno()           {return _trackno;}
    int numtracks()         {return _numtracks;}

    char *genre()           {return _genre;}
    char *originalartist()  {return _originalartist;}
};

class SongReader{

    int _currentframe;
    int _totalframes;

public:
    
    double *buffer;

    SongReader(char *path);
    ~SongReader();

    void next_frame(); //steps to the next frame

    int current_frame()  {return _currentframe;}
    int total_frames()   {return _totalframes;}
};

// simple container class for metadata and reader
class Song{
public:
    SongReader *reader;
    MetaData *metadata;

    Song(char *filename) {
       metadata = new MetaData(filename);
       reader = new SongReader(filename);
    }
};

#endif
