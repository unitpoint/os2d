LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := main

#SDK_ROOT points to folder with SDL and oxygine-framework
LOCAL_SRC_FILES := ../../../../../SDL/src/main/android/SDL_android_main.c

LOCAL_SRC_FILES += ../../../src/example.cpp ../../../src/entry_point.cpp 

LOCAL_SRC_FILES += ../../../../../objectscript/src/objectscript.cpp
LOCAL_SRC_FILES += ../../../../../objectscript/src/os-heap.cpp
LOCAL_SRC_FILES += ../../../../../objectscript/src/ext-datetime/os-datetime.cpp
LOCAL_SRC_FILES += ../../../../../objectscript/src/EaseFunction.cpp

LOCAL_STATIC_LIBRARIES := oxygine-framework_static
LOCAL_SHARED_LIBRARIES := SDL2

LOCAL_CFLAGS += -DOX_WITH_OBJECTSCRIPT
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../objectscript/src

LOCAL_EXPORT_C_INCLUDES += $(LOCAL_C_INCLUDES)
LOCAL_EXPORT_CFLAGS += $(LOCAL_CFLAGS)

include $(BUILD_SHARED_LIBRARY)

#import from NDK_MODULE_PATH defined in build.cmd
$(call import-module, oxygine-framework)
