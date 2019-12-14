//
// Created by max on 12/10/19.
//

#ifndef BEATRUNNER_BEATRUNNER_H
#define BEATRUNNER_BEATRUNNER_H


#include <OpenSource/SuperpoweredAndroidAudioIO.h>
#include <SuperpoweredAdvancedAudioPlayer.h>

class BeatRunner {
public:
    BeatRunner(unsigned int sampleRate, unsigned int bufferSize);
    ~BeatRunner();
    static int OpenFile(const char *path, int offset, int length);

private:
    static SuperpoweredAndroidAudioIO *output;
    static Superpowered::AdvancedAudioPlayer *player;
    unsigned double bpm;

    bool audioProcessing(void *clientData,	// A custom pointer your callback receives.
                         short int *audioIO,	// 16-bit stereo interleaved audio input and/or output.
                         int numFrames,		// The number of frames received and/or requested.
                         int sampleRate);
};


#endif //BEATRUNNER_BEATRUNNER_H
