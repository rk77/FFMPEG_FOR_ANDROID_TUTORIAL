LOCAL_PATH := $(call my-dir)  
  
# FFmpeg library  
include $(CLEAR_VARS)  
LOCAL_MODULE := avcodec  
LOCAL_SRC_FILES := lib/libavcodec-57.so  
include $(PREBUILT_SHARED_LIBRARY)  
  
include $(CLEAR_VARS)  
LOCAL_MODULE := avfilter  
LOCAL_SRC_FILES := lib/libavfilter-6.so  
include $(PREBUILT_SHARED_LIBRARY)  
  
include $(CLEAR_VARS)  
LOCAL_MODULE := avformat  
LOCAL_SRC_FILES := lib/libavformat-57.so  
include $(PREBUILT_SHARED_LIBRARY)  
  
include $(CLEAR_VARS)  
LOCAL_MODULE := avutil  
LOCAL_SRC_FILES := lib/libavutil-55.so  
include $(PREBUILT_SHARED_LIBRARY)  
  
include $(CLEAR_VARS)  
LOCAL_MODULE := swresample  
LOCAL_SRC_FILES := lib/libswresample-2.so  
include $(PREBUILT_SHARED_LIBRARY)  
  
include $(CLEAR_VARS)  
LOCAL_MODULE := swscale  
LOCAL_SRC_FILES := lib/libswscale-4.so  
include $(PREBUILT_SHARED_LIBRARY)  
  
# Program  
include $(CLEAR_VARS)  
LOCAL_MODULE := FFMpegJni 
LOCAL_SRC_FILES := com_rk_myapp_MainActivity.c
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include
LOCAL_LDLIBS := -llog -lz
LOCAL_SHARED_LIBRARIES := avcodec avfilter avformat avutil swresample swscale
include $(BUILD_SHARED_LIBRARY)
