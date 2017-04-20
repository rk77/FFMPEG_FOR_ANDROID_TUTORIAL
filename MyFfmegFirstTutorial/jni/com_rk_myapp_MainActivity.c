#include <jni.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/log.h>

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
int numBytes;
uint8_t *buffer;
AVPacket packet;
int frameFinished;
struct SwsContext *sws_ctx;
char *uri;

void dump_metadata_for_andriod(void *ctx, AVDictionary *m, const char *indent)
{
    if (m && !(av_dict_count(m) == 1 && av_dict_get(m, "language", NULL, 0))) {
        AVDictionaryEntry *tag = NULL;

        LOGI("%sMetadata:\n", indent);
        while ((tag = av_dict_get(m, "", tag, AV_DICT_IGNORE_SUFFIX)))
            if (strcmp("language", tag->key)) {
                const char *p = tag->value;
                LOGI("%s  %-16s: ", indent, tag->key);
                while (*p) {
                    char tmp[256];
                    size_t len = strcspn(p, "\x8\xa\xb\xc\xd");
                    av_strlcpy(tmp, p, FFMIN(sizeof(tmp), len+1));
                    av_log(ctx, AV_LOG_INFO, "%s", tmp);
                    p += len;
                    if (*p == 0xd) av_log(ctx, AV_LOG_INFO, " ");
                    if (*p == 0xa) av_log(ctx, AV_LOG_INFO, "\n%s  %-16s: ", indent, "");
                    if (*p) p++;
                }
                LOGI("\n");
            }
    }
}

void av_dump_format_for_android(AVFormatContext *ic, int index,
                    const char *url, int is_output)
{
    int i;
    uint8_t *printed = ic->nb_streams ? av_mallocz(ic->nb_streams) : NULL;
    if (ic->nb_streams && !printed)
        return;

    LOGI("%s #%d, %s, %s '%s':\n",
           is_output ? "Output" : "Input",
           index,
           is_output ? ic->oformat->name : ic->iformat->name,
           is_output ? "to" : "from", url);
    dump_metadata_for_andriod(NULL, ic->metadata, "  ");

    if (!is_output) {
        LOGI("  Duration: ");
        if (ic->duration != AV_NOPTS_VALUE) {
            int hours, mins, secs, us;
            int64_t duration = ic->duration + (ic->duration <= INT64_MAX - 5000 ? 5000 : 0);
            secs  = duration / AV_TIME_BASE;
            us    = duration % AV_TIME_BASE;
            mins  = secs / 60;
            secs %= 60;
            hours = mins / 60;
            mins %= 60;
            LOGI("%02d:%02d:%02d.%02d", hours, mins, secs,
                   (100 * us) / AV_TIME_BASE);
        } else {
            LOGI("N/A");
        }
        if (ic->start_time != AV_NOPTS_VALUE) {
            int secs, us;
            LOGI(", start: ");
            secs = llabs(ic->start_time / AV_TIME_BASE);
            us   = llabs(ic->start_time % AV_TIME_BASE);
            LOGI("%s%d.%06d",
                   ic->start_time >= 0 ? "" : "-",
                   secs,
                   (int) av_rescale(us, 1000000, AV_TIME_BASE));
        }
        LOGI(", bitrate: ");
        if (ic->bit_rate) {
            LOGI("%"PRId64" kb/s", (int64_t)ic->bit_rate / 1000);
        } else {
            LOGI("N/A");
        }
        LOGI("\n");
    }
}



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
    numBytes = 0;
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
    LOGI("nativePrepare()");
    // Open video file.
    if (avformat_open_input(&pFormatCtx, uri, NULL, NULL) != 0)
    {
        LOGI("nativePrepare(), Couldn't open file.");
        return; // Couldn't open file.
    }

    // Retrieve stream information.
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
    {
        LOGI("nativePrepare(), Couldn't find stream information.");
        return; // Couldn't find stream information.
    }

    // Dump information about file onto standard error.
    av_dump_format_for_android(pFormatCtx, 0, uri, 0);

    // Find the first video stream.
    for (i = 0; i < pFormatCtx->nb_streams; i++)
    {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoStream = i;
            break;
        }
    }
    if (videoStream == -1)
    {
        LOGI("nativePrepare(), Didn't find a video stream.");
        return; // Didn't find a video stream.
    }

    LOGI("nativePrepare(), find all stream num = %d", pFormatCtx->nb_streams);
    // Get a pointer to the codec context for the video stream.
    pCodecCtxOrig = pFormatCtx->streams[videoStream]->codec;
    // Find the decoder for the video stream.
    pCodec = avcodec_find_decoder(pCodecCtxOrig->codec_id);
    if (pCodec == NULL)
    {
        LOGI("nativePrepare(), Unsupported codec!");
        return; // Codec not found.
    }

    // Copy context.
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if (avcodec_copy_context(pCodecCtx, pCodecCtxOrig) != 0)
    {
        LOGI("nativePrepare(), Couldn't copy codec context");
        return; // Error copying codec context.
    }

    // Open codec.
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
    {
        LOGI("nativePrepare(), Could not open codec.");
        return; // Could not open codec.
    }

    // Allocate video frame.
    pFrame = av_frame_alloc();

    // Allocate an AVFrame structure.
    pFrameRGB = av_frame_alloc();
    if (pFrameRGB == NULL)
    {
        LOGI("nativePrepare(), Could not alloc for pFrameRGB.");
        return;
    }

    // Determine required buffer size and allocate buffer.
    numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height, 1);
    buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));

    // Assign appropriate parts of buffer to image planes in pFrameRGB
    //Note that pFrameRGB is an AVFrame, but AVFrame is a superset of AVPicture
    //avpicture_fill((AVPicture *)pFrameRGB, buffer, AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height); // Deprecated.
    av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, buffer, AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height, 1);

    // Initialize SWS context for software scaling.
    sws_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);

}

/*
 * Class:     com_rk_myapp_MainActivity
 * Method:    nativeStart
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_rk_myapp_MainActivity_nativeStart(JNIEnv *env, jobject obj)
{
    LOGI("nativeStart()");
    // Read frames and save first five frames to disk.
    i = 0;
    while (av_read_frame(pFormatCtx, &packet) >= 0)
    {
        // Is this a packet from the video stream?
        if (packet.stream_index == videoStream)
        {
            // Decode video frame
            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);

            // Did we get a video frame?
            if (frameFinished)
            {
                // Convert the image from its native format to RGB.
                sws_scale(sws_ctx, (uint8_t const * const *) pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);

                // Save the frame to disk.
                if (++i <= 0)
                {
                    //TODO: save the frame.
                }
            }
        }

        // Free the packet that was allocated by av_read_frame.
        //av_free_packet(&packet); // Deprecated.
        av_packet_unref(&packet);
    }
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

/*
 * Class:     com_rk_myapp_MainActivity
 * Method:    nativeDeinit
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_rk_myapp_MainActivity_nativeDeinit(JNIEnv *env, jobject obj)
{
    LOGI("nativeDeinit()");
    // Free the RGB image.
    av_free(buffer);
    av_frame_free(&pFrameRGB);

    // Free the YUV frame.
    av_frame_free(&pFrame);

    // Close the codecs.
    avcodec_close(pCodecCtx);
    avcodec_close(pCodecCtxOrig);

    // Close the video file.
    avformat_close_input(&pFormatCtx);
}


