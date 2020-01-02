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

    return ((BeatRunner*)clientData)->timeStretchProcess(audioIO, (unsigned int)sampleRate, (unsigned int)numFrames);
    // return ((BeatRunner*)clientData)->process(audioIO, (unsigned int)sampleRate, (unsigned int)numFrames);
}

bool BeatRunner::timeStretchProcess(short int *audioIO, unsigned int sampleRate, unsigned int numFrames) {
    timeStretcher->rate = 0.7f;
    timeStretcher->samplerate = decoder->getSamplerate();
    float *rawAudioFromPlayer = (float *)malloc(numFrames * 8 + 64);
    // float *processedAudioFromStretcher = (float *)malloc(numFrames * 8 + 64);

//    Superpowered::AudiopointerlistElement *inputBuffer = new Superpowered::AudiopointerlistElement;
//    inputBuffer->firstFrame = 0;
//    inputBuffer->framesUsed = 0;
//    inputBuffer->lastFrame = numFrames;
//    inputBuffer->buffers[0] = (float *)Superpowered::AudiobufferPool::getBuffer(numFrames * 8 + 64);
//    inputBuffer->buffers[1] = inputBuffer->buffers[2] = inputBuffer->buffers[3] = NULL;

    bool playerProcessed = player->processStereo(rawAudioFromPlayer, false, (unsigned int) numFrames);
    if (!playerProcessed) {
        return false;
    }

    // Start stretching: feed the timeStretcher
    timeStretcher->addInput(rawAudioFromPlayer, numFrames);

    Superpowered::AudiopointerlistElement *inputBuffer = new Superpowered::AudiopointerlistElement;
    inputBuffer->firstFrame = 0;
    inputBuffer->framesUsed = 0;
    inputBuffer->lastFrame = numFrames;
    inputBuffer->buffers[0] = Superpowered::AudiobufferPool::getBuffer(numFrames * 8 + 64);
    inputBuffer->buffers[1] = inputBuffer->buffers[2] = inputBuffer->buffers[3] = NULL;

    timeStretcher->getOutput((float *)inputBuffer->buffers[0], numFrames);
    outputBufferList->append(inputBuffer); // Not supposed to be used in real time??
    // Do we have some output?
    if (outputBufferList->makeSlice(0, numFrames)) {
        while (true) {
            int numProcessedFrames = 0;
            float *timeStretchedAudio = (float *)outputBufferList->nextSliceItem(&numProcessedFrames);
            if (!timeStretchedAudio) {
                break;
            }
            Superpowered::FloatToShortInt(timeStretchedAudio, audioIO, (unsigned int) numProcessedFrames);
            audioIO += numProcessedFrames * 2;
            outputBufferList->removeFromStart(numFrames); // ?
        };
    };

    return true;
}

bool BeatRunner::process(short int *audioIO, unsigned int sampleRate, unsigned int numFrames) {
    player->outputSamplerate = sampleRate;
    // audioIO is the buffer the player reads
    // playerOutput is the output buffer that gets fed into audioIO
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
    decoder = new Superpowered::Decoder();
    outputBufferList = new Superpowered::AudiopointerList(8, 16);
    timeStretcher = new Superpowered::TimeStretching(sampleRate);
    Superpowered::AudiobufferPool::initialize();
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
}

BeatRunner::~BeatRunner() {
    delete output;
    delete player;
    delete analyzer;
    delete decoder;
    delete timeStretcher;
}

void BeatRunner::OpenFile(const char *path, int offset, int length) {
    player->open(path, offset, length);
}

void BeatRunner::TogglePlayPause() {
    player->togglePlayback();
    Superpowered::CPU::setSustainedPerformanceMode(player->isPlaying());
}

int BeatRunner::GetBPM(const char *path, int offset, int length) {
    std::string s = path;
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

    FILE *stretchedFile = Superpowered::createWAV("base.wav", decoder->getSamplerate(), 2);
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
    beatRunner->OpenFile(str, offset, length);
    beatRunner->GetBPM(str, offset, length);
    env->ReleaseStringUTFChars(path, str);
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