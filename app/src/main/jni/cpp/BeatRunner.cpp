#include <jni.h>
#include <string>
#include <OpenSource/SuperpoweredAndroidAudioIO.h>
#include <Superpowered.h>
#include <SuperpoweredReverb.h>
#include <SuperpoweredSimple.h>
#include <malloc.h>

static SuperpoweredAndroidAudioIO *audioIO;
static Superpowered::Reverb *reverb;

extern "C" JNIEXPORT void
Java_com_example_beatrunner_MainActivity_StopAudio (JNIEnv * __unused env, jobject __unused obj) {
    delete audioIO;
    delete reverb;
}