#include <BeatRunner.h>
#include <jni.h>
#include <string>
#include <OpenSource/SuperpoweredAndroidAudioIO.h>
#include <Superpowered.h>
#include <SuperpoweredReverb.h>
#include <SuperpoweredSimple.h>
#include <malloc.h>


extern "C" JNIEXPORT void
Java_com_example_beatrunner_SuperPlayer_BeatRunner(
        JNIEnv * __unused env,
        jobject __unused obj,
        jint sampleRate,
        jint bufferSize) {
    // TODO: implement BeatRunner()

}