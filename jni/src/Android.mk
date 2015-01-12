LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

LOCAL_STATIC_LIBRARIES := android_native_app_glue

SDL_PATH := ../SDL

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libpng
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../zlib
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../SDL2_image

# Add your application source files here...
LOCAL_SRC_FILES := \
	$(subst $(LOCAL_PATH)/,, \
	$(wildcard $(LOCAL_PATH)/SDL/*.cpp) \
	$(wildcard $(LOCAL_PATH)/gb/*.cpp) \
	$(wildcard $(LOCAL_PATH)/*.cpp) \
	$(wildcard $(LOCAL_PATH)/*.c) )

LOCAL_STATIC_LIBRARIES += SDL2
LOCAL_STATIC_LIBRARIES += zlib png

LOCAL_LDLIBS += -L$(LOCAL_PATH)/../../obj/local/$(TARGET_ARCH_ABI)
LOCAL_LDLIBS += -L$(LOCAL_PATH)/../SDL/lib/$(TARGET_ARCH_ABI)
LOCAL_LDLIBS += -lGLESv1_CM -llog -lzlib -lGLESv2 -lSDL2 -lOpenSLES -lEGL -landroid -lSDL2_image -lpng
LOCAL_CFLAGS += -DBKPT_SUPPORT -DC_CORE -DSDL -DHAVE_ZUTIL_H -DFINAL_VERSION
LOCAL_CFLAGS += -DNDEBUG -O3
LOCAL_CPPFLAGS += -Wno-write-strings -Wno-overflow

include $(BUILD_SHARED_LIBRARY)
$(call import-module,android/native_app_glue)