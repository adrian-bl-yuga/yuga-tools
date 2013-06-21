LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES:= qc-fqd.c
LOCAL_MODULE := qc-fqd
LOCAL_SHARED_LIBRARIES:= libcutils
include $(BUILD_EXECUTABLE)
