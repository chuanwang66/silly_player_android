LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL
FFMPEG_PATH := ../ffmpeg

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(FFMPEG_PATH)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../include

# Add your application source files here...
# (rename player.c to samSDL2.c)
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.c \
	samSDL2.c \
	global.c \
	packet_queue.c \
	audio.c \
	video.c \
	parse.c \


LOCAL_SHARED_LIBRARIES := SDL2
LOCAL_SHARED_LIBRARIES += avcodec avfilter avformat avutil swresample swscale

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -lz -llog

include $(BUILD_SHARED_LIBRARY)

# FFmpeg library
include $(CLEAR_VARS)
LOCAL_MODULE := avcodec
LOCAL_SRC_FILES := $(FFMPEG_PATH)/libavcodec-56.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := avfilter
LOCAL_SRC_FILES := $(FFMPEG_PATH)/libavfilter-5.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := avformat
LOCAL_SRC_FILES := $(FFMPEG_PATH)/libavformat-56.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := avutil
LOCAL_SRC_FILES := $(FFMPEG_PATH)/libavutil-54.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := swresample
LOCAL_SRC_FILES := $(FFMPEG_PATH)/libswresample-1.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := swscale
LOCAL_SRC_FILES := $(FFMPEG_PATH)/libswscale-3.so
include $(PREBUILT_SHARED_LIBRARY)