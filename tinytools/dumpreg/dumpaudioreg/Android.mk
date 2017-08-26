# Copyright 2005 The Android Open Source Project

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	dump.c

LOCAL_MODULE := dumpamlaudioreg

LOCAL_SHARED_LIBRARIES := libcutils libc
include $(BUILD_EXECUTABLE)

#include $(BUILD_HOST_EXECUTABLE)
