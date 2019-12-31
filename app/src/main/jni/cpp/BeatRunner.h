#ifndef BEATRUNNER_BEATRUNNER_H
#define BEATRUNNER_BEATRUNNER_H

#include <OpenSource/SuperpoweredAndroidAudioIO.h>
#include <SuperpoweredAdvancedAudioPlayer.h>
#include <SuperpoweredAnalyzer.h>
#include <SuperpoweredDecoder.h>
#include <SuperpoweredAudioBuffers.h>
#include <SuperpoweredFilter.h>
#include <SuperpoweredTimeStretching.h>

class BeatRunner {
public:
    BeatRunner(unsigned int sampleRate, unsigned int bufferSize);
    ~BeatRunner();
    bool process(short int *audioIO, unsigned int sampleRate, unsigned int numFrames);
    bool timeStretchProcess(short *audioIO, unsigned int sampleRate, unsigned int numFrames);
    void OpenFile(const char *path, int offset, int length);
    void TogglePlayPause();
    int GetBPM(const char *path, int offset, int length);
    void TimeStretch(const char *path, int offset, int length);


private:
    SuperpoweredAndroidAudioIO *output;
    Superpowered::AdvancedAudioPlayer *player;
    Superpowered::Analyzer *analyzer;
    Superpowered::Decoder *decoder;
    Superpowered::TimeStretching *timeStretcher;
    Superpowered::AudiopointerList *audioPointerList;
};

#endif //BEATRUNNER_BEATRUNNER_H
