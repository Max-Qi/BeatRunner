#include <BeatRunner.h>
#include <Superpowered.h>
#include <OpenSource/SuperpoweredAndroidAudioIO.h>
#include <malloc.h>
#include <android/log.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_AndroidConfiguration.h>
#include <jni.h>
#include <string>
#include <SuperpoweredSimple.h>

bool BeatRunner::audioProcessing(
    void * __unused clientData,	// A custom pointer your callback receives.
    short int *audioIO,	// 16-bit stereo interleaved audio input and/or output.
    int numFrames,		// The number of frames received and/or requested.
    int sampleRate	    // The current sample rate in Hz.
) {
    player->outputSamplerate = sampleRate;
    float playerOutput [numFrames * 2];
    if (player->processStereo(playerOutput, false, (unsigned int) numFrames)) {
        Superpowered::FloatToShortInt(playerOutput, audioIO, (unsigned int) numFrames);
        return true;
    } else {
        return false;
    }
}

BeatRunner::BeatRunner(
    unsigned int sampleRate,
    unsigned int bufferSize) {
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
        NULL,
        -1,
        -1
    );
}

BeatRunner::OpenFile(const char *path, int offset, int length) {
    player->open(path, offset, length);

}

BeatRunner::~BeatRunner() {
    delete output;
    delete player;
}

extern "C" JNIEXPORT void
Java_com_example_beatrunner_SuperPlayer_BeatRunnerInit(
    JNIEnv * __unused env,
    jobject __unused obj,
    jint sampleRate,
    jint bufferSize) {

    BeatRunner((unsigned int)sampleRate, (unsigned int)bufferSize);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_beatrunner_SuperPlayer_OpenFile(
    JNIEnv *env,
    jobject thiz,
    jstring path,
    jint offset,
    jint length) {

    const char *str = env->GetStringUTFChars(path, 0);
    BeatRunner::OpenFile(str, offset, length);
    env->ReleaseStringUTFChars(path, str);
}