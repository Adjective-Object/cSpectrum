#include <cstdio>
#include "song.h"
#include <SDL2/SDL_mutex.h>
using namespace std;

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/samplefmt.h>
#include <libswscale/swscale.h>

#include <libavformat/avformat.h>

}

#define MAX_AUDIO_FRAME_SIZE 192000 // jesus that's big
#define TEMP_BUFFER_SIZE MAX_AUDIO_FRAME_SIZE * 2
uint16_t temp_buffer[TEMP_BUFFER_SIZE];


static void exitWithAvError(int av_err_no) {
    char errmsg[128];
    av_strerror(av_err_no, (char *) &errmsg, 128);
    cerr << "error opening codec contexxt" << endl;
    exit(1);
}

void readPacketsToQueue(AVFormatContext *formatContext,
                        queue<AVPacket *> & queue, AVStream *stream) {

    AVPacket readingPacket;
    av_init_packet(&readingPacket);

    // constantly read packaets into readingPacket
    int loopct = 0;
    while (av_read_frame(formatContext, &readingPacket) == 0) {
        loopct++;
        // if they are packets we care about
        if (readingPacket.stream_index == stream->index) {
            // copy them to a new block and push to the queue
            AVPacket *currentPacket = (AVPacket *) malloc(sizeof(AVPacket));
            memcpy(currentPacket, &readingPacket, sizeof(AVPacket));
            queue.push(currentPacket);
            cout << "inserting packet" << endl;
        }
    }
    cout << "loopct: " << loopct << endl;
    cout << "streamIndex: " << stream->index << endl;
}

AVPacket *grabFrameFromQueue(AVCodecContext* codec_context,
                        AVFrame* frame,
                        AVPacket *current_packet,
                        queue<AVPacket*> & packet_queue){
    // cout << ".";
    int gotFrame = 0;
    while(not gotFrame) {
        // if we're not currently processing a packet, grab one off the
        // front of the queue
        if (current_packet == nullptr) {
            cout << "grabbing new packet off the queue" << endl;
            // try to grab the front of the packet queue
            // if it fails, panic and exit
            if (packet_queue.size() > 0) {
                current_packet = packet_queue.front();
                packet_queue.pop();
            }
            else {
                cerr << "no packets in queue!" << endl;
                exit(1);
            }
        }

        // try to decode a frame.
        int result = avcodec_decode_audio4(
                codec_context,
                frame,
                &gotFrame,
                current_packet);

        // if we got a frame sucessfully, keep it around to pull from next time
        if (gotFrame) {
            current_packet->size -= result;
            current_packet->data += result;
            if (current_packet->size == 0) {
                return NULL;
            }
        }
        // if there was no error, discard the current packet and try again
        else if (result >= 0){
            av_free_packet(current_packet);
            current_packet = nullptr;
        }
        // failing that, throw it away and print the error
        else {
            char * errbuf = static_cast<char *>(malloc (sizeof(char) * 1000));
            av_strerror(result, errbuf, 1000);
            cerr << errbuf << endl;
            exit(1);
        }
    }
    return current_packet;
}

SongReader::SongReader(AVFormatContext *formatContext) {
    // intialize counter variables
    _currentframe = 0;
    _totalframes = 0; //TODO calculate song size
    _current_packet = nullptr;


    _format_context = formatContext;

    buffer_maxsize = TEMP_BUFFER_SIZE;
    size_t bufsize = sizeof(double) * buffer_maxsize;
    buffer_head = (uint8_t *) malloc(bufsize);
    output_buffer = buffer_head;
    memset(buffer_head, bufsize, 0);
    buffer_tail = buffer_head + buffer_maxsize;
    buffer_data_tail = buffer_head;
    buffer_data_head = buffer_head;

    // initialize the audio stream
    AVCodec* cdc = nullptr;
    int streamIndex = av_find_best_stream(
            formatContext, AVMEDIA_TYPE_AUDIO, -1, -1, &cdc, 0);

    if (streamIndex < 0) {
        cerr << "Could not find any audio stream in the file" << endl;
        exit(1);
    }

    AVStream* audioStream = formatContext->streams[streamIndex];
    AVCodecContext* aCodecCtxOrig = audioStream->codec;
    this->_input_codec = avcodec_find_decoder(aCodecCtxOrig->codec_id);
    if(!this->_input_codec) {
        fprintf(stderr, "Unsupported codec!\n");
        exit(1);
    }

    this->_codec_context = avcodec_alloc_context3(this->_input_codec);
    if(avcodec_copy_context(this->_codec_context, aCodecCtxOrig) != 0) {
        fprintf(stderr, "Couldn't copy codec context");
        exit(1);
    }

    this->_codec_context->sample_fmt = AV_SAMPLE_FMT_S16;

    if (avcodec_open2(this->_codec_context,
                      this->_input_codec,
                      nullptr) < 0) {
        cerr << "Couldn't open the context with the decoder" << endl;
        exit(1);
    }

    // read contents of stream into the packet queue
    readPacketsToQueue(formatContext, this->_pkt_queue, audioStream);
    cout << "Read Packets :" << this->_pkt_queue.size() << endl;
}

SongReader::~SongReader(){
	free(buffer_head);
}

void SongReader::next_frame() {
    AVFrame * frame = av_frame_alloc();

    // if there is no room left in the buffer, copy the remainder to the head
    cout << "head: " << static_cast<void *>(buffer_data_head)
    << " maxsize: " <<  TEMP_BUFFER_SIZE
    << " tail: " << buffer_data_head + TEMP_BUFFER_SIZE
    << " current tail: " << buffer_tail
    << " remaining space:" <<
            static_cast<size_t>(buffer_tail - buffer_data_tail)
    << endl;

    if (buffer_data_head + TEMP_BUFFER_SIZE > buffer_tail) {
        memcpy(buffer_head,
               buffer_data_head,
               (buffer_data_tail - buffer_data_head));
        cout << "moving data back to make room" << endl;
        buffer_data_tail -= (buffer_data_head - buffer_head);
        buffer_data_head = buffer_head;
    }

    // grab frame data until we can fill the SPECTRUM BUFSIZE
    while(buffer_data_tail - buffer_data_head < SPECTRUM_BUFFSIZE) {
        cout << "pulling new frame of data ("
        << SPECTRUM_BUFFSIZE - (buffer_data_tail - buffer_data_head)
        << " bytes left)" << endl;

        this->_current_packet = grabFrameFromQueue(
                this->_codec_context,
                frame,
                this->_current_packet,
                this->_pkt_queue);

        // calculate size of the buffer required for this frame
        int data_size = av_samples_get_buffer_size(
                NULL,
                this->_codec_context->channels,
                frame->nb_samples,
                this->_codec_context->sample_fmt,
                1);

        cout << "grabbed " << data_size << " bytes of audio" << endl;
        memcpy(buffer_data_tail, frame->data[0], data_size);
        buffer_data_tail += data_size;

        if (data_size >  buffer_tail - buffer_data_tail) {
            cerr << "no room left in buffer for this frame" << endl;
            cerr << "remaining buffer space: "
                 << buffer_data_tail - output_buffer << endl;
            cerr << "frame samples: " << frame->nb_samples << endl;
            exit(1);
        }

        //TODO
        //handle planar and packed audio differently
        // if planar, the AVFrame is a linked list of audio frames
        // if packed, then it's

        av_frame_unref(frame);
    }
    cout << "data grab complete" << endl;

    // shift old data out of buffer
    buffer_data_head = buffer_data_head + SPECTRUM_BUFFSIZE;

    av_frame_unref(frame);
    av_frame_free(&frame);
}


MetaData::MetaData(AVFormatContext *ctx) {
    //TODO this
}

MetaData::~MetaData() {

}

Song::Song(char *filename) {

    AVFormatContext *ctx = NULL;
    avformat_open_input(&ctx, filename, NULL, 0);

    avformat_find_stream_info(ctx, NULL);

    metadata = new MetaData(ctx);
    reader = new SongReader(ctx);
}

Song::~Song() {
    delete reader;
    delete metadata;
}