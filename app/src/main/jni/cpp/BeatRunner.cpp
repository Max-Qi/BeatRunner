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
        false, // enableAudioTimeStretching (using SuperpoweredTimeStretching)
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
//    analyzer = new Superpowered::Analyzer(sampleRate, -1);
//    decoder = new Superpowered::Decoder();
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

double BeatRunner::GetBPM(const char *path, int offset, int length) {
    decoder = new Superpowered::Decoder;
    analyzer = new Superpowered::Analyzer(decoder->getSamplerate(), decoder->getDurationSeconds());
    filter = new Superpowered::Filter(Superpowered::Resonant_Lowpass, decoder->getSamplerate());
    int open = decoder->open(path, false, offset, length);

    filter->frequency = 1000.0f;
    filter->resonance = 0.1f;
    filter->enabled = true;

    short int *intBuffer = (short int *)malloc(decoder->getFramesPerChunk() * 2 * sizeof(short int) + 16384);
    float *floatBuffer = (float *)malloc(decoder->getFramesPerChunk() * 2 * sizeof(float) + 16384);

    while(true) {
        unsigned int framesDecoded = decoder->decodeAudio(intBuffer, decoder->getFramesPerChunk());
        if (framesDecoded < 1) {
            break;
        }
        Superpowered::ShortIntToFloat(intBuffer, floatBuffer, framesDecoded);
        filter->process(floatBuffer, floatBuffer, framesDecoded);
        // analyzer->process(floatBuffer, framesDecoded);
    }

    analyzer->makeResults(60, 200, 0, 0, false, false, false, false, false);
    int bpm = analyzer->bpm;
}

static BeatRunner *beatRunner = NULL;

extern "C" JNIEXPORT void
Java_com_example_beatrunner_SuperPlayer_BeatRunnerInit(JNIEnv * __unused env, jobject __unused obj, jint sampleRate, jint bufferSize) {
    beatRunner = new BeatRunner((unsigned int)sampleRate, (unsigned int)bufferSize);
}

extern "C" JNIEXPORT void
Java_com_example_beatrunner_SuperPlayer_OpenFile(JNIEnv *env, jobject thiz, jstring path, jint offset, jint length) {
    const char *str = env->GetStringUTFChars(path, 0);
     beatRunner->OpenFile(str, offset, length);
    beatRunner->GetBPM(str, offset, length);
    env->ReleaseStringUTFChars(path, str);
}

extern "C" JNIEXPORT void
Java_com_example_beatrunner_SuperPlayer_TogglePlayPause(JNIEnv *env, jobject thiz) {
    beatRunner->TogglePlayPause();
}
