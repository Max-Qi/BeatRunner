cmake_minimum_required(VERSION 3.4.1)

# convert SDK path to forward slashes on Windows
file(TO_CMAKE_PATH ${PATH_TO_SUPERPOWERED} PATH_TO_SUPERPOWERED)

include_directories(src/main/jni/cpp)
include_directories(${PATH_TO_SUPERPOWERED})

# compile crossfader example to a native library
add_library(
	BeatRunner
	SHARED
	src/main/jni/cpp/BeatRunner.cpp
	${PATH_TO_SUPERPOWERED}/OpenSource/SuperpoweredAndroidAudioIO.cpp
)

# link the native library against the following libraries
target_link_libraries(
	BeatRunner
	log
	android
	OpenSLES
	${PATH_TO_SUPERPOWERED}/libSuperpoweredAndroid${ANDROID_ABI}.a
)
