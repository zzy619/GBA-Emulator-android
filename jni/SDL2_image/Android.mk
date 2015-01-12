LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := SDL2_image
LOCAL_STATIC_LIBRARIES := android_native_app_glue

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../SDL/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libpng

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

LOCAL_SRC_FILES := \
	$(subst $(LOCAL_PATH)/,, \
    $(wildcard $(LOCAL_PATH)/*.c))

LOCAL_CFLAGS += -DGL_GLEXT_PROTOTYPES -DLOAD_PNG -DLOAD_BMP
LOCAL_LDLIBS += -L$(LOCAL_PATH)/../../obj/local/$(TARGET_ARCH_ABI)
LOCAL_LDLIBS += -L$(LOCAL_PATH)/../SDL/lib/$(TARGET_ARCH_ABI)
LOCAL_LDLIBS += -ldl -llog -landroid -lSDL2 -lGLESv2 -lGLESv1_CM -lOpenSLES
LOCAL_CPPFLAGS += -D__GXX_EXPERIMENTAL_CXX0X__
LOCAL_CPPFLAGS += -fexceptions -frtti -x c++ -std=c++11 -fpermissive

include $(BUILD_STATIC_LIBRARY)
$(call import-module,android/native_app_glue)

