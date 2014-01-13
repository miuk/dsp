#include <math.h>
#include <iostream>
using namespace std;
#include "wav.hxx"

static void
usage(void)
{
    cerr << "usage: mkwav <options>" << endl
         << " -t sine|square|triangle|sawtooth" << endl
         << " -r <sampling rate in Hz>" << endl
         << " -f <frequence in Hz>" << endl
         << " -g <gain 0.0 - 1.0>" << endl
         << " -n <length in seconds>" << endl
         << " -o <output file>" << endl
        ;
    exit(1);
}

static void
make_sine(char* buf, double f, double gain, int offset_sample, int rate, int nsample)
{
    for (int i = 0; i < nsample; i++) {
        double r = 2 * M_PI * f * (double)(i + offset_sample) / (double)rate;
        short a = (short)(32767 * gain * sin(r));
        *(short*)(buf+i*2) = a;
    }
}

enum WaveType {
    WaveType_none,
    WaveType_sine,
    WaveType_square,
    WaveType_triangle,
    WaveType_sawtooth,
};

int
main(int ac, char* av[])
{
    WaveType type = WaveType_sine;
    int rate = 16000;
    double freq = 400;
    double gain = 0.5;
    int sec = 1;
    const char* fname = NULL;
    for (int i = 1; i < ac; i++) {
        if (strcmp(av[i], "-t") == 0) {
            i++;
            if (strcmp(av[i], "sine") == 0) {
                type = WaveType_sine;
            } else if (strcmp(av[i], "square") == 0) {
                type = WaveType_square;
            } else if (strcmp(av[i], "triangle") == 0) {
                type = WaveType_triangle;
            } else if (strcmp(av[i], "sawtooth") == 0) {
                type = WaveType_sawtooth;
            } else {
                usage();
            }
        } else if (strcmp(av[i], "-r") == 0) {
            i++;
            rate = atoi(av[i]);
        } else if (strcmp(av[i], "-f") == 0) {
            i++;
            freq = atof(av[i]);
        } else if (strcmp(av[i], "-g") == 0) {
            i++;
            gain = atof(av[i]);
            if (gain < 0 || gain > 1)
                usage();
        } else if (strcmp(av[i], "-n") == 0) {
            i++;
            sec = atoi(av[i]);
        } else if (strcmp(av[i], "-o") == 0) {
            i++;
            fname = av[i];
        }
    }
    if (fname == NULL)
        usage();
    WAVFmt fmt(WAVFmt::Type_pcm, 1, rate, 16);
    WAV wav;
    string errmsg;
    if (wav.open(fname, fmt, errmsg) == false) {
        cerr << errmsg << endl;
        exit(1);
    }
    int nsample = rate * sec;
    int nbyte = fmt.calcBytes(nsample);
    cout << "nsample: " << nsample << endl;
    cout << "nbyte: " << nbyte << endl;
    char* buf = new char[nbyte];
    make_sine(buf, freq, gain, 0, rate, nsample);
    int len = wav.write(buf, nsample);
    cout << "write len: " << len << endl;
    wav.close(errmsg);
}
