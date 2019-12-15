#ifndef BEATRUNNER_BEATRUNNER_H
#define BEATRUNNER_BEATRUNNER_H

#include <OpenSource/SuperpoweredAndroidAudioIO.h>
#include <SuperpoweredAdvancedAudioPlayer.h>

class BeatRunner {
public:
    BeatRunner(unsigned int sampleRate, unsigned int bufferSize);
    ~BeatRunner();
    bool process(short int *audioIO, unsigned int sampleRate, unsigned int numFrames);
    void OpenFile(const char *path, int offset, int length);
    void TogglePlayPause();


private:
    SuperpoweredAndroidAudioIO *output;
    Superpowered::AdvancedAudioPlayer *player;
    double bpm;
};

#endif //BEATRUNNER_BEATRUNNER_H
