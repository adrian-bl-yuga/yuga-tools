LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	qc-opt.c

LOCAL_SHARED_LIBRARIES := liblog
LOCAL_MODULE := libqc-opt

include $(BUILD_SHARED_LIBRARY)
