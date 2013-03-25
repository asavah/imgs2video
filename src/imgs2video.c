#define _GNU_SOURCE
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <dirent.h>
#include <sys/stat.h>
#include <assert.h>
#include <errno.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavcodec/avcodec.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/buffersrc.h>
#include <libavfilter/buffersink.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
#include "imgs2video_cmdline.h"
#include "compat.h"

#define FMT_TIMEBASE_DEN 1000

struct img {
    char *filename;
    time_t ts;
    unsigned int duration;
};

struct transcoder {
    struct args args;
    struct img *files;
    unsigned int n_files;
    struct img *frames;
    unsigned int n_frames;
    AVFormatContext *out;
    AVCodecContext *enc;
    AVFilterContext *filter_src;
    AVFilterContext *filter_sink;
    AVFilterGraph *filter_graph;
    unsigned int frames_in; // how many have been read
    unsigned int frames_out;
    unsigned int pts_step;
    unsigned int width;
    unsigned int height;
};
typedef struct transcoder Transcoder;

static int global_init(void);
static int tc_build_frames_table(Transcoder *tc);
static int open_out(Transcoder *tc);
static int open_encoder(Transcoder *tc);
static int setup_filters(Transcoder *tc);
static int tc_process_frame(Transcoder *tc, unsigned int i);
static int tc_flush_encoder(Transcoder *tc);

int main(int argc, char **argv) {
    int r;
    int failure = 0;
    Transcoder *tc;
    unsigned int i;

    r = global_init();
    assert(!r);

    tc = calloc(1, sizeof(*tc));
    assert(tc);


    r = cmdline_parser(argc, argv, &tc->args);
    if (r) {
        cmdline_parser_print_help();
        return r;
    }
    if (!strcmp(tc->args.loglevel_arg, "debug"))
        av_log_set_level(AV_LOG_DEBUG);
    else if (!strcmp(tc->args.loglevel_arg, "quiet"))
        av_log_set_level(AV_LOG_QUIET);
    else  /* if (!strcmp(tc->args.loglevel_arg, "normal")) */
        av_log_set_level(AV_LOG_INFO);

    r = tc_build_frames_table(tc);
    if (r)
        return r;

    r = open_out(tc);
    if (r) {
        fprintf(stderr, "Open out file fail\n");
        return r;
    }

    r = open_encoder(tc);
    if (r) {
        fprintf(stderr, "Encoder open fail\n");
        return r;
    }

    r = setup_filters(tc);
    if (r) {
        fprintf(stderr, "Filters setup fail\n");
        return r;
    }

    r = avformat_write_header(tc->out, NULL);
    if (r) {
        fprintf(stderr, "write out file fail\n");
        return r;
    }

    for(i = 0; i < tc->n_frames; i++) {
        printf("processing frame %d/%d\n", i+1, tc->n_frames);
        if (i > 0)
            assert(tc->frames[i].ts > tc->frames[i-1].ts);
        r = tc_process_frame(tc, i);
        if (r < 0) {
            fprintf(stderr, "Fatal error processing frame, aborting\n");
            failure = 1;
            break;
        }
        if (r) {
            fprintf(stderr, "Processing file %s/%s failed, throw away and proceed\n", tc->args.images_dir_arg, tc->frames[i].filename);
        }
    }
    tc_flush_encoder(tc);

    av_write_trailer(tc->out);
    av_dump_format(tc->out, 0, tc->args.output_file_arg, 1);

    avcodec_close(tc->enc);
    avio_close(tc->out->pb);
    avformat_free_context(tc->out);
    free(tc);

    return failure;
}

static int global_init(void) {
    av_register_all();
    avfilter_register_all();
    return 0;
}

static int imgs_names_durations(Transcoder *tc);
static int transform_frames_chain(Transcoder *tc, struct img *array, unsigned int n, struct img **arg);

static int tc_build_frames_table(Transcoder *tc) {
    int r;
    tc->pts_step = FMT_TIMEBASE_DEN / tc->args.frame_rate_arg;
    r = imgs_names_durations(tc);
    if (r)
        return r;
    tc->n_frames = transform_frames_chain(tc, tc->files, tc->n_files, &tc->frames);
    printf("%d frames\n", tc->n_frames);
    return 0;
}

static int open_out(Transcoder *tc) {
    /* auto detect the output format from the name */
    AVOutputFormat *fmt;
    fmt = av_guess_format(NULL, tc->args.output_file_arg, NULL);
    if (!fmt) {
        fprintf(stderr, "Could not find suitable output format\n");
        return 1;
    }
    if (strcmp(fmt->name, "flv")) {
        fprintf(stderr, "Fix the code to support formats other than FLV\n");
        return 1;
    }

    /* allocate the output media context */
    tc->out = avformat_alloc_context();
    if (!tc->out) {
        fprintf(stderr, "Memory error\n");
        return 1;
    }
    tc->out->oformat = fmt;
    snprintf(tc->out->filename, sizeof(tc->out->filename), "%s", tc->args.output_file_arg);

    if (avio_open(&tc->out->pb, tc->out->filename, AVIO_FLAG_WRITE) < 0) {
        fprintf(stderr, "Could not open '%s'\n", tc->out->filename);
        return 1;
    }
    return 0;
}

static int open_encoder(Transcoder *tc) {
    if (tc->out->oformat->video_codec == CODEC_ID_NONE) {
        printf("guessed format doesnt assume video?\n");
        return 1;
    }

    AVCodec *codec = avcodec_find_encoder_by_name(tc->args.vcodec_arg);
    if (!codec) {
        fprintf(stderr, "Encoder %s not found\n", tc->args.vcodec_arg);
        return 1;
    }
    AVStream *st;
    st = avformat_new_stream(tc->out, codec);
    if (!st) {
        fprintf(stderr, "Could not alloc stream\n");
        return 1;
    }

    // for flv format, timebase is 1kHz
    st->time_base.den = FMT_TIMEBASE_DEN; // FIXME hardcode
    st->time_base.num = 1;
    st->sample_aspect_ratio.den = 1;
    st->sample_aspect_ratio.num = 1;

    tc->enc = st->codec;
    /* resolution must be a multiple of two */
    tc->enc->width = tc->args.in_width_arg;
    tc->enc->height = tc->args.in_height_arg;

    // for video codecs, at last libx264, timebase must be <framerate> Hz
    tc->enc->time_base.den = tc->args.frame_rate_arg;
    tc->enc->time_base.num = 1;
    tc->enc->pix_fmt = PIX_FMT_YUV420P;

    if(tc->out->oformat->flags & AVFMT_GLOBALHEADER)
        tc->enc->flags |= CODEC_FLAG_GLOBAL_HEADER;

    tc->enc->sample_aspect_ratio.den = 1;
    tc->enc->sample_aspect_ratio.num = 1;

    tc->enc->bit_rate = tc->args.bitrate_arg;
    tc->enc->bit_rate_tolerance = tc->enc->bit_rate / 5;
    tc->enc->thread_count = 0; // use several threads for encoding

    AVDictionary *opts = NULL;
    if (tc->args.bitrate_arg != 0) // profiles don't support lossless
        av_dict_set(&opts, "profile", tc->args.profile_arg, 0);
    else
        av_dict_set(&opts, "qp", "0", 0); // set lossless mode
    av_dict_set(&opts, "preset", tc->args.preset_arg, 0);
    /* open the codec */
    if (avcodec_open2(tc->enc, codec, &opts) < 0) {
        fprintf(stderr, "could not open codec\n");
        return 1;
    }

    av_dump_format(tc->out, 0, tc->args.output_file_arg, 1);
    return 0;
}

static int setup_filters(Transcoder *tc) {
    int r;
    AVFilter *src  = avfilter_get_by_name("buffer");
    assert(src);
    AVFilter *sink = avfilter_get_by_name("buffersink");
    assert(sink);
    AVFilterInOut *outputs = avfilter_inout_alloc();
    assert(outputs);
    AVFilterInOut *inputs  = avfilter_inout_alloc();
    assert(inputs);
    tc->filter_graph = avfilter_graph_alloc();
    assert(tc->filter_graph);

    char filter_args[50];
    /* buffer video source: the decoded frames from the decoder will be inserted here. */
    snprintf(filter_args, sizeof(filter_args), "%d:%d:%d:%d:%d:%d:%d",
            tc->args.in_width_arg, tc->args.in_height_arg, PIX_FMT_YUVJ420P /* FIXME UNHARDCODE */,
            tc->enc->time_base.num, tc->enc->time_base.den,
            tc->enc->sample_aspect_ratio.num, tc->enc->sample_aspect_ratio.den);
    r = avfilter_graph_create_filter(&tc->filter_src, src, "in",
            filter_args, NULL, tc->filter_graph);
    if (r < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot create buffer source\n");
        return r;
    }

    /* buffer video sink: to terminate the filter chain. */
    enum PixelFormat pix_fmts[] = { PIX_FMT_YUV420P, PIX_FMT_NONE };
    AVBufferSinkParams *buffersink_params = av_buffersink_params_alloc();
    buffersink_params->pixel_fmts = pix_fmts;
    r = avfilter_graph_create_filter(&tc->filter_sink, sink, "out",
            NULL, buffersink_params, tc->filter_graph);
    av_free(buffersink_params);
    if (r < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot create buffer sink\n");
        return r;
    }

    /* Endpoints for the filter graph. */
    outputs->name       = av_strdup("in");
    outputs->filter_ctx = tc->filter_src;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;

    inputs->name       = av_strdup("out");
    inputs->filter_ctx = tc->filter_sink;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;

    char *filter_descr = tc->args.filter_arg;

    r = avfilter_graph_parse(tc->filter_graph, filter_descr,
#ifdef LIBAV
            inputs, outputs,
#else
            &inputs, &outputs,
#endif
            NULL);

    if (r < 0)
        return r;

    if ((r = avfilter_graph_config(tc->filter_graph, NULL)) < 0)
        return r;

    return 0;
}

static int tc_process_frame_input(Transcoder *tc, unsigned int i);
static int tc_process_frame_output(Transcoder *tc);

/**
 * @return 0 on success, <0 on fatal error, 1 on non-fatal error
 */
static int tc_process_frame(Transcoder *tc, unsigned int i) {
    int r;
    r = tc_process_frame_input(tc, i);
    if (r) {
        fprintf(stderr, "tc_process_frame_input fail\n");
        return r;
    }
    r = tc_process_frame_output(tc);
    if (r) {
        fprintf(stderr, "tc_process_frame_output fail\n");
        return r;
    }
    return 0;
}

static int tc_write_encoded(Transcoder *tc, AVPacket *pkt);

static int tc_flush_encoder(Transcoder *tc) {
    // TODO flush filter
    int r;
    while (1) {
        /* flush buffered remainings */
        int got_packet = 0;
        AVPacket pkt;
        av_init_packet(&pkt);
        pkt.data = NULL;
        pkt.size = 0;
        r = avcodec_encode_video2(tc->enc, &pkt, NULL, &got_packet);
        if ((r < 0) | !got_packet)
            break;
        tc_write_encoded(tc, &pkt);
    }
    return 0;
}

/**
 * @return 0 on success, <0 on fatal error, 1 on non-fatal error
 */
static int tc_process_frame_input(Transcoder *tc, unsigned int i) {
    struct img *img = &tc->frames[i];
    AVFormatContext *pFormatCtx = NULL;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    AVFrame *pFrame;
    AVPacket packet;
    int r;
    int frameFinished;

    if(avformat_open_input(&pFormatCtx, img->filename, NULL, 0)) {
        fprintf(stderr, "Can't open image file '%s'\n", img->filename);
        goto fail_open_file;
    }

    r = avformat_find_stream_info(pFormatCtx, NULL);
    if (r < 0) {
        fprintf(stderr, "Failed recognizing image file '%s'\n", img->filename);
        goto fail_avcodec_open;
    }
    //av_dump_format(pFormatCtx, 0, img->filename, 0);

    pCodecCtx = pFormatCtx->streams[0]->codec;

    if (pCodecCtx->width != tc->args.in_width_arg) {
        fprintf(stderr, "Image file '%s' width %d does not match, must be %d\n", img->filename, pCodecCtx->width, tc->args.in_width_arg);
        goto fail_avcodec_open;
    }
    if (pCodecCtx->height != tc->args.in_height_arg) {
        fprintf(stderr, "Image file '%s' height %d does not match, must be %d\n", img->filename, pCodecCtx->height, tc->args.in_height_arg);
        goto fail_avcodec_open;
    }
    if (pCodecCtx->pix_fmt != PIX_FMT_YUVJ420P /* FIXME UNHARDCODE */) {
        fprintf(stderr, "Image file '%s' pix_fmt %s does not match, must be %s\n", img->filename, av_get_pix_fmt_name(pCodecCtx->pix_fmt), av_get_pix_fmt_name(PIX_FMT_YUVJ420P));
        goto fail_avcodec_open;
    }

    // Find the decoder for the video stream
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (!pCodec) {
        fprintf(stderr, "Codec not found\n");
        goto fail_find_decoder;
    }

    // Open codec
    AVDictionary *opts = NULL;
    av_dict_set(&opts, "err_detect", "explode", 0);
    r = avcodec_open2(pCodecCtx, pCodec, &opts);
    av_dict_free(&opts);
    if(r < 0) {
        fprintf(stderr, "Could not open codec\n");
        goto fail_avcodec_open;
    }

    pFrame = avcodec_alloc_frame();
    if (!pFrame) {
        fprintf(stderr, "Can't allocate memory for AVFrame\n");
        goto fail_alloc_frame;
    }

    r = av_read_frame(pFormatCtx, &packet);
    if (r < 0) {
        fprintf(stderr, "Failed to read frame\n");
        goto fail_read_frame;
    }

    r = avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
    if (r <= 0) {
        fprintf(stderr, "Failed to decode image\n");
        goto fail_decode;
    }
    pFrame->pts = tc->frames_in++; // for encoding, pts step must be exactly 1
    pFrame->pict_type = 0; /* let codec choose */

    /* push the decoded frame into the filtergraph */
    r = av_buffersrc_add_frame(tc->filter_src, pFrame, 0);
    assert(r >= 0);

    av_free(pFrame);
    av_free_packet(&packet);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);

    return 0;
fail_decode:
    av_free_packet(&packet);
fail_read_frame:
    av_free(pFrame);
fail_alloc_frame:
    avcodec_close(pCodecCtx);
fail_avcodec_open:
    ;
fail_find_decoder:
    avformat_close_input(&pFormatCtx);
fail_open_file:
    return 1;
}

/**
 * @return 0 on success, <0 on fatal error, 1 on non-fatal error
 */
static int tc_process_frame_output(Transcoder *tc) {
    int r;
    int out_size;

    /* pull filtered pictures from the filtergraph */
    AVFilterBufferRef *picref = NULL;
    r = av_buffersink_get_buffer_ref(tc->filter_sink, &picref, 0);
    if (r < 0) {
        if (r == AVERROR(EAGAIN))
            return 0;
        fprintf(stderr, "av_buffersink_get_buffer_ref fail %d\n", r);
        return r;
    }
    assert(picref);

    AVFrame *picture = avcodec_alloc_frame();
    assert(picture);
    r = avfilter_copy_buf_props(picture, picref);
    assert(r == 0);
    picture->pts = picref->pts;
    /* encode the image */
    int got_packet = 0;
    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;
    r = avcodec_encode_video2(tc->enc, &pkt, picture, &got_packet);
    avfilter_unref_buffer(picref);

    /* if zero size, it means the image was buffered */
    if (!got_packet) {
        av_log(NULL, AV_LOG_VERBOSE, "encoded frame, no data out, filling encoder buffer\n");
        return 0;
    }
    r = tc_write_encoded(tc, &pkt);
    if (r) {
        fprintf(stderr, "Error while writing video frame\n");
        return -1;
    }
    return 0;
}

static int tc_write_encoded(Transcoder *tc, AVPacket *pkt) {
    int ret;
    pkt->pts = tc->frames_out++ * tc->pts_step;
    pkt->dts = pkt->pts;
    pkt->duration = tc->pts_step;
    ret = av_write_frame(tc->out, pkt);
    av_free_packet(pkt);
    return ret;
}


static int match_postfix_jpg(const char *filename) {
    char *p = strcasestr(filename, ".jpg");
    if (!p)
        return 0;
    if (!strcasecmp(p, ".jpg"))
        return 1; // exact postfix match
    else
        return 0;
}

static int filter_jpg(const struct dirent *a) {
    return match_postfix_jpg(a->d_name);
}

static int compare_mod_dates(const struct dirent **a, const struct dirent **b) {
    struct stat a_stat, b_stat;
    int r;
    r = stat((*a)->d_name, &a_stat);
    if (r != 0) {
        fprintf(stderr, "stat for '%s' failed: ret %d, errno %d '%s'\n",
                (*a)->d_name, r, errno, strerror(errno));
        exit(1);
    }
    r = stat((*b)->d_name, &b_stat);
    if (r != 0) {
        fprintf(stderr, "stat for '%s' failed: ret %d, errno %d '%s'\n",
                (*b)->d_name, r, errno, strerror(errno));
        exit(1);
    }

    return a_stat.st_mtime - b_stat.st_mtime;
}

static int transform_frames_chain(Transcoder *tc, struct img *array, unsigned int n, struct img **arg) {
    // TODO better algo?
    unsigned idx(struct img *frames, unsigned n_frames, unsigned timestamp) {
        unsigned best_i = 0;
        unsigned int i;
        for (i = 0; i < n_frames; i++) {
            if (FFABS((int64_t)frames[i].ts - (int64_t)timestamp) <
                    FFABS((int64_t)frames[best_i].ts - (int64_t)timestamp) )
                best_i = i;
        }
        return best_i;
    }
    unsigned int realtime_duration = array[n-1].ts - array[0].ts;
    unsigned int n_frames = realtime_duration * tc->args.frame_rate_arg / tc->args.speedup_coef_arg;
    struct img *frames = calloc(n_frames, sizeof(struct img));
    unsigned int i, j;

    for (i = 0; i < n_frames; i++) {
        frames[i].ts = i * tc->pts_step;
        //printf("frame[%d].ts := %d\n", i, i * pts_step);
        frames[i].duration = tc->pts_step;
    }

    frames[0].filename = array[0].filename;
    frames[n_frames-1].filename = array[n-1].filename;

    for (i = 0; i < n-1; i++) {
        /* for each image:
         * set the stop point at the cell in frames[] which is for next image
         * fill all frames[] from which is for it, up to which is for next
         */
        unsigned entry_pos = idx(frames, n_frames, (array[i].ts-array[0].ts) * FMT_TIMEBASE_DEN / tc->args.speedup_coef_arg);
        unsigned next_entry_pos = idx(frames, n_frames, (array[i+1].ts-array[0].ts) * FMT_TIMEBASE_DEN / tc->args.speedup_coef_arg);
        //printf("img %d, ts %"PRId64", position %d, of next is %d\n", i, (array[i].ts-array[0].ts) * FMT_TIMEBASE_DEN / args.speedup_coef_arg, entry_pos, next_entry_pos);
        for (j = entry_pos; j < next_entry_pos; j++)
            frames[j].filename = array[i].filename;
    }

    for (i = 0; i < n_frames; i++)
        assert(frames[i].filename);

    *arg = frames;
    return n_frames;
}

static int imgs_names_durations(Transcoder *tc) {
    const char *dir = tc->args.images_dir_arg;

    /*
     * 1. List directory files
     * 2. Sort by mod date, ascending
     * 3. Calc intervals
     */

    struct dirent **namelist;
    struct stat st;
    char *cwd;
    unsigned int i;
    int r;

    cwd = getcwd(NULL, 0);
    assert(cwd);
    r = chdir(dir);
    if (r) {
        fprintf(stderr, "Failed to chdir to '%s': errno %d\n", dir, errno);
        exit(1);
    }

    tc->n_files = scandir(".", &namelist, filter_jpg, compare_mod_dates);
    if (tc->n_files == 0) {
        fprintf(stderr, "source dir contains no suitable files\n");
        return 1;
    }

    r = chdir(cwd);
    assert(!r);

    tc->files = calloc(tc->n_files, sizeof(struct img));

    for (i = 0; i < tc->n_files; i++) {
        asprintf(&tc->files[i].filename, "%s/%s", dir, namelist[i]->d_name);
        free(namelist[i]);
        r = stat(tc->files[i].filename, &st);
        if (r != 0) {
            fprintf(stderr, "stat for '%s' failed: ret %d, errno %d '%s'\n",
                    tc->files[i].filename, r, errno, strerror(errno));
            return 1;
        }
        tc->files[i].ts = st.st_mtime;
        if (i > 0)
            tc->files[i].duration = tc->files[i].ts - tc->files[i-1].ts;
        //printf("%s %u\n", tc->files[i].filename, tc->files[i].duration);
    }
    free(namelist);
    av_log(NULL, AV_LOG_VERBOSE, "%d images\n", tc->n_files);
    return 0;
}

