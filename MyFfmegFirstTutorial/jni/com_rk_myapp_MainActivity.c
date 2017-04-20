#include <jni.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>

#include <android/log.h>

#define LOG_TAG "FFMpeg_JNI"
#define LOGI(...) __android_log_print(4, LOG_TAG, __VA_ARGS__);

AVFormatContext *pFormatCtx;
int i, videoStream;
AVCodecContext *pCodecCtxOrig;
AVCodecContext *pCodecCtx;
AVCodec *pCodec;
AVFrame *pFrame;
AVFrame *pFrameRGB;
int numBypes;
uint8_t *buffer;
AVPacket packet;
int frameFinished;
struct SwsContext *sws_ctx;
char *uri;


/*
 * Class:     com_rk_myapp_MainActivity
 * Method:    nativeInit
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_rk_myapp_MainActivity_nativeInit(JNIEnv *env, jobject obj)
{
    pFormatCtx = NULL;
    i = 0;
    videoStream = -1;
    pCodecCtxOrig = NULL;
    pCodecCtx = NULL;
    pCodec = NULL;
    pFrame = NULL;
    pFrameRGB = NULL;
    numBypes = 0;
    buffer = NULL;
    frameFinished = 0;
    sws_ctx = NULL;
    uri = NULL;
    LOGI("nativeInit()"); 
    av_register_all();
}

/*
 * Class:     com_rk_myapp_MainActivity
 * Method:    nativeSetSurface
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_rk_myapp_MainActivity_nativeSetSurface(JNIEnv *env, jobject obj)
{
}

/*
 * Class:     com_rk_myapp_MainActivity
 * Method:    nativeSetUri
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_rk_myapp_MainActivity_nativeSetUri(JNIEnv *env, jobject obj, jstring filePath)
{
    uri = (*env)->GetStringUTFChars(env, filePath, JNI_FALSE);
    LOGI("nativeSetUri(), uri = %s", uri); 
}

/*
 * Class:     com_rk_myapp_MainActivity
 * Method:    nativeGetFrameBitmap
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_rk_myapp_MainActivity_nativeGetFrameBitmap(JNIEnv *env, jobject obj, jint frame)
{
}

/*
 * Class:     com_rk_myapp_MainActivity
 * Method:    nativePrepare
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_rk_myapp_MainActivity_nativePrepare(JNIEnv *env, jobject obj)
{
}

/*
 * Class:     com_rk_myapp_MainActivity
 * Method:    nativeStart
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_rk_myapp_MainActivity_nativeStart(JNIEnv *env, jobject obj)
{
}

/*
 * Class:     com_rk_myapp_MainActivity
 * Method:    nativePause
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_rk_myapp_MainActivity_nativePause(JNIEnv *env, jobject obj)
{
}

/*
 * Class:     com_rk_myapp_MainActivity
 * Method:    nativePlay
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_rk_myapp_MainActivity_nativePlay(JNIEnv *env, jobject obj)
{
}

/*
 * Class:     com_rk_myapp_MainActivity
 * Method:    nativeSeek
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_rk_myapp_MainActivity_nativeSeek(JNIEnv *env, jobject obj, jint position)
{
}
