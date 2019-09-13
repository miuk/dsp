#include <stdio.h>
#include <signal.h>
#include <alsa/asoundlib.h>

#include "wav.hxx"

static bool bRunning = true;

static void
intr_handler(int arg)
{
    bRunning = false;
}


int
main(int ac, char* av[])
{
    snd_pcm_stream_t stream = SND_PCM_STREAM_CAPTURE;
    //const char* pcm_name = "plughw:0,0";
    const char* pcm_name = "default";
    snd_pcm_hw_params_t* hwparams;
    snd_pcm_hw_params_alloca(&hwparams);
    snd_pcm_t* pcm_handle;
    int err;
    if ((err = snd_pcm_open(&pcm_handle, pcm_name, stream, 0)) < 0) {
        fprintf(stderr, "Error opening PCM device %s: %s\n"
                , pcm_name, snd_strerror(err));
        exit(1);
    }
    if ((err = snd_pcm_hw_params_any(pcm_handle, hwparams)) < 0) {
        fprintf(stderr, "snd_pcm_hw_params_any: %s\n", snd_strerror(err));
        exit(1);
    }
    if ((err = snd_pcm_hw_params_set_access(pcm_handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
        fprintf(stderr, "snd_pcm_hw_params_set_access: %s\n", snd_strerror(err));
        exit(1);
    }
    snd_pcm_format_t fmt = SND_PCM_FORMAT_S16_LE;
    //snd_pcm_format_t fmt = SND_PCM_FORMAT_MU_LAW;
    if ((err = snd_pcm_hw_params_set_format(pcm_handle, hwparams, fmt)) < 0) {
        fprintf(stderr, "snd_pcm_hw_params_set_format: %d: %s\n"
                , fmt, snd_strerror(err));
        exit(1);
    }
    unsigned int rate = 8000;
    if ((err = snd_pcm_hw_params_set_rate_near(pcm_handle, hwparams, &rate, 0)) < 0) {
        fprintf(stderr, "snd_pcm_hw_params_set_rate_near: %d: %s\n"
                , rate, snd_strerror(err));
        exit(1);
    }
    printf("rate=%d\n", rate);
    int nch = 1;
    if ((err = snd_pcm_hw_params_set_channels(pcm_handle, hwparams, nch)) < 0) {
        fprintf(stderr, "snd_pcm_hw_params_set_channels: %d, %s\n"
                , nch, snd_strerror(err));
        exit(1);
    }
//    if (snd_pcm_hw_params_set_periods(pcm_handle, hwparams, 2, 0) < 0) {
//        fprintf(stderr, "snd_pcm_hw_params_set_periods\n");
//        exit(1);
//    }
    int framesize = 200;
    if ((err = snd_pcm_hw_params_set_buffer_size(pcm_handle, hwparams, framesize)) < 0) {
        fprintf(stderr, "snd_pcm_hw_params_set_buffer_size: %s\n"
                , snd_strerror(err));
        exit(1);
    }
    if ((err = snd_pcm_hw_params(pcm_handle, hwparams)) < 0) {
        fprintf(stderr, "snd_pcm_hw_params: %s\n", snd_strerror(err));
        exit(1);
    }

    if ((err = snd_pcm_prepare(pcm_handle)) < 0) {
        fprintf(stderr, "snd_pcm_prepare: %s\n", snd_strerror(err));
        exit(1);
    }

    signal(SIGINT, intr_handler);

    FILE* fp = fopen("rec.raw", "w");
    int cnt = 0;
    while (bRunning) {
        int16_t buf[framesize];
        int ret = snd_pcm_readi(pcm_handle, buf, framesize);
        if (ret < 0) {
            fprintf(stderr, "snd_pcm_readi: %s\n", snd_strerror(ret));
            break;
        }
        fwrite(buf, sizeof(int16_t), ret, fp);
        printf("%d read %d\n", cnt, ret);
        cnt++;
    }
    fclose(fp);

    snd_pcm_close(pcm_handle);
}
