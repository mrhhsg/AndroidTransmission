#include <jni.h>
#include <stdio.h>
#include <pthread.h>

extern int tr_main(int argc, char *argv[]);

static void*
tr_thread(void *arg) {
    (void)arg;
    tr_main(0, NULL);
    return NULL;
}

extern int Java_com_sg_atransmission_Transmission_start(JNIEnv *env, jclass class) {
    pthread_t tid;
    pthread_create(&tid, NULL, tr_thread, NULL);
    return 0;
}
