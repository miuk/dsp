#include <stdio.h>
#include <iostream>
using namespace std;

#include "fft.hxx"
#include "wav.hxx"

void
usage(void)
{
    cerr << "Usage: fftest -n <n> fname" << endl
         << " n must be power of 2" << endl;
    exit(1);
}

bool
isPowerOf2(int n)
{
    if (n < 2)
        return false;
    int x = 2;
    int p = 0;
    for (unsigned int x = 2; x <= 0x40000000; x <<= 1) {
        if ((n & x) != 0) {
            p++;
            if (p > 1)
                return false;
        }
    }
    return true;
}

bool
readWav(WAV& wav, short* buf, int n)
{
    const WAVFmt& fmt = wav.getFmt();
    int nread = 0;
    switch (fmt.type) {
    case WAVFmt::Type_pcm : {
        if (fmt.nch == 1) {
            nread = wav.read((char*)buf, n);
        } else if (fmt.nch == 2) {
            short* tmp = new short[n*2];
            nread = wav.read((char*)tmp, n);
            for (int i = 0; i < nread; i++) {
                buf[i] = tmp[i*2];
            }
        } else {
            return false;
        }
    }
        break;
    case WAVFmt::Type_mulaw :
        break;
    default :
        return false;
    }

    for (; nread < n; nread++)
        buf[nread] = 0;
    return true;
}

int
main(int ac, char* av[])
{
    const char* fname = NULL;
    int n = 0;
    for (int i = 1; i < ac; i++) {
        if (!strcmp(av[i], "-n")) {
            i++;
            n = atoi(av[i]);
        } else if (av[i][0] != '-') {
            fname = av[i];
        } else {
            usage();
        }
    }
    if (fname == NULL)
        usage();
    if (!isPowerOf2(n))
        usage();

    WAV wav;
    string errmsg;
    if (!wav.open(fname, errmsg)) {
        cerr << errmsg << endl;
        exit(1);
    }
    const WAVFmt& fmt = wav.getFmt();
    short* buf = new short[n];
    if (!readWav(wav, buf, n)) {
        cerr << "wav read failed." << endl;
        wav.close(errmsg);
        delete[] buf;
        exit(1);
    }

    FFT fft(n);
    double* x = fft.s2d(buf);
    double* y = new double[n];
    for (int i = 0; i < n; i++)
        y[i] = 0;
    fft.hanning(x);
    fft.fft(x, y);
    double* a = fft.abs(x, y);
    for (int i = 0; i < n; i++)
        cout << a[i] << endl;

    wav.close(errmsg);
    delete[] buf;
    exit(0);
}
