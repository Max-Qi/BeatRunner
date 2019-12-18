#ifndef BEATRUNNER_BEATRUNNER_H
#define BEATRUNNER_BEATRUNNER_H

#include <OpenSource/SuperpoweredAndroidAudioIO.h>
#include <SuperpoweredAdvancedAudioPlayer.h>
#include <SuperpoweredAnalyzer.h>
#include <SuperpoweredDecoder.h>
#include <SuperpoweredAudioBuffers.h>
#include <SuperpoweredFilter.h>

class BeatRunner {
public:
    BeatRunner(unsigned int sampleRate, unsigned int bufferSize, const char *path, int offset, int length);
    ~BeatRunner();
    bool process(short int *audioIO, unsigned int sampleRate, unsigned int numFrames);
    void OpenFile(const char *path, int offset, int length);
    void TogglePlayPause();
    double GetBPM(const char *path, int offset, int length);


private:
    SuperpoweredAndroidAudioIO *output;
    Superpowered::AdvancedAudioPlayer *player;
    Superpowered::Analyzer *analyzer;
    Superpowered::Decoder *decoder;
    Superpowered::Filter *filter;

};

#endif //BEATRUNNER_BEATRUNNER_H
