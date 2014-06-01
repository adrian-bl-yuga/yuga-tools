LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES:= waldi.c
LOCAL_MODULE := waldi
LOCAL_SHARED_LIBRARIES:= libcutils
include $(BUILD_EXECUTABLE)
