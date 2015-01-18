#include <jni.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <libtransmission/transmission.h>

#define LOG_TAG "TransmissionJNI"

#include <native_log.h>

#define UNUSED(x) (void)(x)

extern "C" {
extern void setUserConfigDir(const char *dir);
extern int tr_checkConfig();
extern void tr_main(int argc, char *argv[]);
}

static const char *const gClassName = "com/sg/atransmission/jni/Transmission";
static const char *const gOnStateChangeCallbackName = "onStateChanged";

static jmethodID gOnStateChangeCallbackMethodID;

static jboolean
Java_com_sg_atransmission_jni_Transmission_init(JNIEnv *env, jclass clazz) {
    UNUSED(env);
    UNUSED(clazz);
    return JNI_TRUE;
}

static void
Java_com_sg_atransmission_jni_Transmission_deinit(JNIEnv *env, jclass clazz) {
    UNUSED(env);
    UNUSED(clazz);
}

static void
Java_com_sg_atransmission_jni_Transmission_set_conf_path(JNIEnv *env, jobject object, jstring path) {
    const char *c_path = env->GetStringUTFChars(path, NULL);
    UNUSED(object);
    if (c_path == NULL) {
        LOGE("cannot get path's cstring\n");
        return;
    }

    LOGD("now set the conf path to: %s\n", c_path);
    setUserConfigDir(c_path);
    env->ReleaseStringUTFChars(path, c_path);
}

static jboolean
Java_com_sg_atransmission_jni_Transmission_check_conf(JNIEnv *env, jobject object) {
    if (!tr_checkConfig()) {
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

static void* Thread(void *arg) {
    UNUSED(arg);

    tr_main(0, NULL);
    return NULL;
}

static jboolean
Java_com_sg_atransmission_jni_Transmission_start(JNIEnv *env, jobject object) {
    pthread_t tid;
    if (0 != pthread_create(&tid, NULL, Thread, NULL)) {
        LOGD("cannot create the main thread\n");
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

static JNINativeMethod nativeMethods[] = {
    {"native_init", "()Z", (void*)Java_com_sg_atransmission_jni_Transmission_init},
    {"native_deinit", "()V", (void*)Java_com_sg_atransmission_jni_Transmission_deinit},
    {"native_set_conf_path", "(Ljava/lang/String;)V", (void*)Java_com_sg_atransmission_jni_Transmission_set_conf_path},
    {"native_check_conf", "()Z", (void*)Java_com_sg_atransmission_jni_Transmission_check_conf},
    {"native_start", "()Z", (void*)Java_com_sg_atransmission_jni_Transmission_start},
};

#define NELEM(x) (sizeof(x) / sizeof((x)[0]))

static int register_jni_functions(JNIEnv *env) {
    jclass clazz = env->FindClass(gClassName);
    return env->RegisterNatives(clazz, nativeMethods, NELEM(nativeMethods));
}

static void _init(JNIEnv *env) {
    jclass clazz = env->FindClass(gClassName);
    gOnStateChangeCallbackMethodID = env->GetMethodID (clazz, gOnStateChangeCallbackName, "(I)V");
}

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv *env = NULL;
    (void)reserved;
    jint result = -1;

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
       LOGD("cannot get Java Env\n");
    }

    assert(env != NULL);
    register_jni_functions(env);
    _init(env);
    result = JNI_VERSION_1_4;
    return result;
}
