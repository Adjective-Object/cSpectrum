#ifndef SPECTRUM_SONG_INCLUDED
#define SPECTRUM_SONG_INCLUDED

#include <iostream>
#include <stdlib.h>
#include <cstdio>
#include <sys/unistd.h>
#include <vector>
#include <queue>

#include <SDL2/SDL_mutex.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

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

    MetaData(AVFormatContext *ctx);
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
    AVPacket *_current_packet;

    AVCodec *_input_codec;
    AVCodecContext *_codec_context;
    AVFormatContext *_format_context;
    std::queue<AVPacket*> _pkt_queue;
    int _tracking_stream;

    void parse_all_frames();

    uint8_t *buffer_head,
            *buffer_tail,
            *buffer_data_head,
            *buffer_data_tail;

    size_t buffer_maxsize;

public:

    uint8_t *output_buffer;

    SongReader(AVFormatContext *formatContext);
    ~SongReader();

    void next_frame(); //gets the next frame
    void start_reading();

    int current_frame()  {return _currentframe;}
    int total_frames()   {return _totalframes;}
};

// simple container class for metadata and reader
class Song{
public:
    SongReader *reader;
    MetaData *metadata;
    Song(char *filename);
    ~Song();
};

#endif
