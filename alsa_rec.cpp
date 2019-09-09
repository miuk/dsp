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
    if (snd_pcm_open(&pcm_handle, pcm_name, stream, 0) < 0) {
        fprintf(stderr, "Error opening PCM device %s\n", pcm_name);
        exit(1);
    }
    if (snd_pcm_hw_params_any(pcm_handle, hwparams) < 0) {
        fprintf(stderr, "snd_pcm_hw_params_any\n");
        exit(1);
    }
    if (snd_pcm_hw_params_set_access(pcm_handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
        fprintf(stderr, "snd_pcm_hw_params_set_access\n");
        exit(1);
    }
    snd_pcm_format_t fmt = SND_PCM_FORMAT_S16_LE;
    //snd_pcm_format_t fmt = SND_PCM_FORMAT_MU_LAW;
    if (snd_pcm_hw_params_set_format(pcm_handle, hwparams, fmt) < 0) {
        fprintf(stderr, "snd_pcm_hw_params_set_format: %d\n", fmt);
        exit(1);
    }
    unsigned int rate = 8000;
    if (snd_pcm_hw_params_set_rate_near(pcm_handle, hwparams, &rate, 0) < 0) {
        fprintf(stderr, "snd_pcm_hw_params_set_rate_near: %d\n\n", rate);
        exit(1);
    }
    printf("rate=%d\n", rate);
    int nch = 1;
    if (snd_pcm_hw_params_set_channels(pcm_handle, hwparams, nch) < 0) {
        fprintf(stderr, "snd_pcm_hw_params_set_channels: %d\n", nch);
        exit(1);
    }
//    if (snd_pcm_hw_params_set_periods(pcm_handle, hwparams, 2, 0) < 0) {
//        fprintf(stderr, "snd_pcm_hw_params_set_periods\n");
//        exit(1);
//    }
    int bps = 2 * nch;
    int framesize = 512 / bps;
    if (snd_pcm_hw_params_set_buffer_size(pcm_handle, hwparams, framesize) < 0) {
        fprintf(stderr, "snd_pcm_hw_params_set_buffer_size\n");
        exit(1);
    }
    if (snd_pcm_hw_params(pcm_handle, hwparams) < 0) {
        fprintf(stderr, "snd_pcm_hw_params\n");
        exit(1);
    }

    signal(SIGINT, intr_handler);

    int cnt = 0;
    while (bRunning) {
        char buf[512];
        int ret;
        while ((ret = snd_pcm_readi(pcm_handle, buf, framesize)) < 0) {
            snd_pcm_prepare(pcm_handle);
            fprintf(stderr, "buffer overrun\n");
        }
        printf("%d read %d\n", cnt, ret);
        cnt++;
    }
}
