# Copyright 2016 The Fuchsia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

LOCAL_DIR := $(GET_LOCAL_DIR)
MODULE := $(LOCAL_DIR)
MODULE_TYPE := userlib

MODULE_SRCS = $(LOCAL_DIR)/new.cpp \
              $(LOCAL_DIR)/pure_virtual.cpp

MODULE_SO_NAME := mxcpp

MODULE_LIBS += ulib/musl

include make/module.mk
