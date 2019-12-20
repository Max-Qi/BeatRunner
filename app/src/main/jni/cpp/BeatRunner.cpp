#include <iostream>
#include <BeatRunner.h>
#include <Superpowered.h>
#include <SuperpoweredSimple.h>
#include <SuperpoweredCPU.h>
#include <malloc.h>
#include <android/log.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_AndroidConfiguration.h>
#include <jni.h>
#include <string>
#include <cmath>
#include <SuperpoweredTimeStretching.h>

#define log_print __android_log_print

static bool audioProcessing(
    void * __unused clientData,	// A custom pointer your callback receives.
    short int *audioIO,	// 16-bit stereo interleaved audio input and/or output.
    int numFrames,		// The number of frames received and/or requested.
    int sampleRate	    // The current sample rate in Hz.
) {
    return ((BeatRunner*)clientData)->process(audioIO, (unsigned int)sampleRate, (unsigned int)numFrames);
}

bool BeatRunner::process(short int *audioIO, unsigned int sampleRate, unsigned int numFrames) {
    player->outputSamplerate = sampleRate;
    float playerOutput [numFrames * 2];
    if (player->processStereo(playerOutput, false, (unsigned int) numFrames)) {
        Superpowered::FloatToShortInt(playerOutput, audioIO, (unsigned int) numFrames);
        return true;
    } else {
        return false;
    }
}

BeatRunner::BeatRunner(unsigned int sampleRate, unsigned int bufferSize) {
    Superpowered::Initialize(
        "ExampleLicenseKey-WillExpire-OnNextUpdate",
        true, // enableAudioAnalysis (using SuperpoweredAnalyzer, SuperpoweredLiveAnalyzer, SuperpoweredWaveform or SuperpoweredBandpassFilterbank)
        false, // enableFFTAndFrequencyDomain (using SuperpoweredFrequencyDomain, SuperpoweredFFTComplex, SuperpoweredFFTReal or SuperpoweredPolarFFT)
        true, // enableAudioTimeStretching (using SuperpoweredTimeStretching)
        false,  // enableAudioEffects (using any SuperpoweredFX class)
        true,  // enableAudioPlayerAndDecoder (using SuperpoweredAdvancedAudioPlayer or SuperpoweredDecoder)
        false, // enableCryptographics (using Superpowered::RSAPublicKey, Superpowered::RSAPrivateKey, Superpowered::hasher or Superpowered::AES)
        false  // enableNetworking (using Superpowered::httpRequest)
    );
    player = new Superpowered::AdvancedAudioPlayer(sampleRate, 0);
    output = new SuperpoweredAndroidAudioIO (
        sampleRate,
        bufferSize,
        false,
        true,
        audioProcessing,
        this,
        -1,
        -1
    );
    decoder = new Superpowered::Decoder();
}

BeatRunner::~BeatRunner() {
    delete output;
    delete player;
    delete analyzer;
    delete decoder;
    delete filter;
}

void BeatRunner::OpenFile(const char *path, int offset, int length) {
    player->open(path, offset, length);
}

void BeatRunner::TogglePlayPause() {
    player->togglePlayback();
    Superpowered::CPU::setSustainedPerformanceMode(player->isPlaying());
}

int BeatRunner::GetBPM(const char *path, int offset, int length) {
    player->open(path, offset, length);
    int open = decoder->open(path, false, offset, length);
    analyzer = new Superpowered::Analyzer(decoder->getSamplerate(), (int) decoder->getDurationSeconds());
    short int *intBuffer = (short int *)malloc(decoder->getFramesPerChunk() * 4 * sizeof(short int) + 16384);
    float *floatBuffer = (float *)malloc(decoder->getFramesPerChunk() * 4 * sizeof(float) + 16384);

    while(true) {
        unsigned int framesDecoded = decoder->decodeAudio(intBuffer, decoder->getFramesPerChunk());
        if (framesDecoded < 1) {
            break;
        }
        Superpowered::ShortIntToFloat(intBuffer, floatBuffer, framesDecoded);
        analyzer->process(floatBuffer, framesDecoded);
    }
    free(intBuffer);
    free(floatBuffer);
    analyzer->makeResults(60, 200, 0, 0, false, false, false, false, false);
    int bpm = (int) std::round(analyzer->bpm);
    return bpm;
}

void BeatRunner::TimeStretch(const char *path, int offset, int length) {
    int open = decoder->open(path, false, offset, length);
    timeStretcher = new Superpowered::TimeStretching(decoder->getSamplerate());
    short int *intBuffer = (short int *)malloc(decoder->getFramesPerChunk() * 4 * sizeof(short int) + 16384);
    float *floatBuffer = (float *)malloc(decoder->getFramesPerChunk() * 4 * sizeof(float));

    timeStretcher->rate = 1.50f;
    FILE *stretchedFile = Superpowered::createWAV("150.wav", decoder->getSamplerate(), 2);
    if (!stretchedFile) {
        return;
    }
    while (true) {
        unsigned int framesDecoded = decoder->decodeAudio(intBuffer, decoder->getFramesPerChunk());
        if (framesDecoded < 1) {
            break;
        }
        Superpowered::ShortIntToFloat(intBuffer, floatBuffer, framesDecoded);
        timeStretcher->addInput(floatBuffer, framesDecoded);

        unsigned int framesAvailable = timeStretcher->getOutputLengthFrames();
        if (framesAvailable > 0 && timeStretcher->getOutput(floatBuffer, framesAvailable)) {
            Superpowered::FloatToShortInt(floatBuffer, intBuffer, framesAvailable);
            Superpowered::writeWAV(stretchedFile, intBuffer, framesAvailable * 4);
        }
    }

    Superpowered::closeWAV(stretchedFile);
    delete decoder;
    delete timeStretcher;
    free(intBuffer);
    free(floatBuffer);
}

static BeatRunner *beatRunner = NULL;

extern "C" JNIEXPORT void
Java_com_example_beatrunner_SuperPlayer_BeatRunnerInit(JNIEnv * __unused env, jobject __unused obj, jint sampleRate, jint bufferSize, jstring path) {
    beatRunner = new BeatRunner((unsigned int)sampleRate, (unsigned int)bufferSize);
}

extern "C" JNIEXPORT void
Java_com_example_beatrunner_SuperPlayer_OpenFile(JNIEnv *env, jobject thiz, jstring path, jint offset, jint length) {
    const char *str = env->GetStringUTFChars(path, 0);
    // beatRunner->OpenFile(str, offset, length);
    beatRunner->GetBPM(str, offset, length);
}

extern "C" JNIEXPORT void
Java_com_example_beatrunner_SuperPlayer_TogglePlayPause(JNIEnv *env, jobject thiz) {
    beatRunner->TogglePlayPause();
}

extern "C" JNIEXPORT void
Java_com_example_beatrunner_SuperPlayer_TimeStretch(JNIEnv *env, jobject thiz, jstring path, jint offset, jint length) {
    const char *str = env->GetStringUTFChars(path, 0);
    beatRunner->TimeStretch(str, offset, length);
    env->ReleaseStringUTFChars(path, str);
}