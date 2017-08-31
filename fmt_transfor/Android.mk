# Copyright 2005 The Android Open Source Project

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	fmt_transfor.c

LOCAL_MODULE:= fmt_trans

LOCAL_SHARED_LIBRARIES :=

include $(BUILD_HOST_EXECUTABLE)
