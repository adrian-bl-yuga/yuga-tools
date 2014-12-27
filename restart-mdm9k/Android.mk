LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES:= restart-mdm9k.c
LOCAL_MODULE := restart-mdm9k
LOCAL_SHARED_LIBRARIES:= libcutils
include $(BUILD_EXECUTABLE)
