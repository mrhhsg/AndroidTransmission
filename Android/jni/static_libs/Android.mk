LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libcrypto.a

LOCAL_SRC_FILES := libcrypto.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libcurl.a

LOCAL_SRC_FILES := libcurl.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libevent.a

LOCAL_SRC_FILES := libevent.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libevent_core.a

LOCAL_SRC_FILES := libevent_core.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libevent_extra.a

LOCAL_SRC_FILES := libevent_extra.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libevent_pthreads.a

LOCAL_SRC_FILES := libevent_pthreads.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libssl.a

LOCAL_SRC_FILES := libssl.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libtransmission.a

LOCAL_SRC_FILES := libtransmission.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libminiupnp.a

LOCAL_SRC_FILES := libminiupnp.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libdht.a

LOCAL_SRC_FILES := libdht.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libutp.a

LOCAL_SRC_FILES := libutp.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libnatpmp.a

LOCAL_SRC_FILES := libnatpmp.a

include $(PREBUILT_STATIC_LIBRARY)
