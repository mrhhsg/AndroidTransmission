#ifndef _DHICAM_COMMON_H_
#define _DHICAM_COMMON_H_

#ifdef __ANDROID__
#   include <android/log.h>
#   ifndef LOG_TAG
#       error "LOG_TAG undefined"
#   endif
#   define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#   define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#else
#   define LOGD(...) printf(__VA_ARGS__)
#   define LOGE(...) printf(__VA_ARGS__)
#endif

#endif
