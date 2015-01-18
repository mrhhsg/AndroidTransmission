LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := "transmission_jni"

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_SRC_FILES := jni_interface.cpp daemon.c watch.c

LOCAL_STATIC_LIBRARIES := libtransmission.a \
	libevent_pthreads.a libevent_extra.a libevent_core.a libevent.a \
	libminiupnp.a libdht.a libutp.a libnatpmp.a \
	libcurl.a libssl.a libcrypto.a

LOCAL_LDLIBS := -lz -llog

include $(BUILD_SHARED_LIBRARY)

include $(LOCAL_PATH)/static_libs/Android.mk
