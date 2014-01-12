#include <math.h>
#include <stdio.h>
#include "fft.hxx"

FFT::FFT(int n)
{
    this->n = n;
    BITREV = new int[n+1];
    int i = 0;
    int j = 0;
    int n2 = n >> 1;
    for (;;) {
        BITREV[i] = j;
        if (++i >= n)
            break;
        int k = n2;
        while (k <= j) {
            j -= k;
            k >>= 1;
        }
        j += k;
    }
    SINTBL = new double[n+1];
    n2 = n >> 1;
    int n4 = n >> 2;
    int n8 = n >> 3;
    double t = sin(M_PI / n);
    double dc = 2 * t * t;
    double ds = sqrt(dc * (2 - dc));
    t = 2 * dc;
    double c = SINTBL[n4] = 1;
    double s = SINTBL[0] = 0;
    for (int i = 1; i < n8; i++) {
        c -= dc;
        dc += t * c;
        s += ds;
        ds -= t * s;
        SINTBL[i] = s;
        SINTBL[n4 - i] = c;
    }
    if (n8 != 0) {
        SINTBL[n8] = sqrt(0.5);
    }
    for (int i = 0; i < n4; i++) {
        SINTBL[n2 - i] = SINTBL[i];
    }
    for (int i = 0; i < n2 + n4; i++) {
        SINTBL[i + n2] = - SINTBL[i];
    }
    COSTBL = new double[n];
    for (int i = 0; i < n; i++) {
        double t = (double)i / (double)n;
        COSTBL[i] = cos(2 * M_PI * t);
    }
}

FFT::~FFT(void)
{
    delete[] BITREV;
    delete[] SINTBL;
    delete[] COSTBL;
}

double*
FFT::s2d(const short* src)
{
    double* dst = new double[n];
    for (int i = 0; i < n; i++) {
        dst[i] = (double)src[i] / 32767.0;
    }
    return dst;
}

short*
FFT::d2s(const double* src)
{
    short* dst = new short[n];
    for (int i = 0; i < n; i++) {
        dst[i] = (short)(src[i] * 32767.0);
    }
    return dst;
}

void
FFT::fft(double* x, double* y)
{
    int n4 = n >> 2;
    for (int i = 0; i < n; i++) {
        int j = BITREV[i];
        if (i < j) {
            double t = x[i]; x[i] = x[j]; x[j] = t;
            t = y[i]; y[i] = y[j]; y[j] = t;
        }
    }
    int k2 = 0;
    int d = n;
    for (int k = 1; k < n; k = k2) {
        int h = 0;
        k2 = k << 1;
        d <<= 1;
        for (int j = 0; j < k; j++) {
            double c = SINTBL[h + n4];
            double s = SINTBL[h];
            for (int i = j; i < n; i += k2) {
                int ik = i + k;
                double dx = s * y[ik] + c * x[ik];
                double dy = c * y[ik] - s * x[ik];
                x[ik] = x[i] - dx;
                x[i] += dx;
                y[ik] = y[i] - dy;
                y[i] += dy;
            }
            h += d;
        }
    }
    for (int i = 0; i < n; i++) {
        x[i] /= n;
        y[i] /= n;
    }
}

void
FFT::ifft(double* x, double* y)
{
    int n4 = n >> 2;
    for (int i = 0; i < n; i++) {
        int j = BITREV[i];
        if (i < j) {
            double t = x[i]; x[i] = x[j]; x[j] = t;
            t = y[i]; y[i] = y[j]; y[j] = t;
        }
    }
    int k2 = 0;
    int d = n;
    for (int k = 1; k < n; k = k2) {
        int h = 0;
        k2 = k << 1;
        d <<= 1;
        for (int j = 0; j < k; j++) {
            double c = SINTBL[h + n4];
            double s = -SINTBL[h];
            for (int i = j; i < n; i += k2) {
                int ik = i + k;
                double dx = s * y[ik] + c * x[ik];
                double dy = c * y[ik] - s * x[ik];
                x[ik] = x[i] - dx;
                x[i] += dx;
                y[ik] = y[i] - dy;
                y[i] += dy;
            }
            h += d;
        }
    }
}

double*
FFT::abs(const double* x, const double* y, bool bHalf)
{
    int _n = (bHalf) ? n >> 1 : n;
    double* a = new double[_n];
    for (int i = 0; i < _n; i++) {
        a[i] = sqrt(x[i]*x[i] + y[i]*y[i]);
    }
    return a;
}

double*
FFT::angle(const double* x, const double* y, bool bHalf)
{
    int _n = (bHalf) ? n >> 1 : n;
    double* a = new double[_n];
    for (int i = 0; i < _n; i++) {
        a[i] = atan2(y[i], x[i]);
    }
    return a;
}

void
FFT::hamming(double* x)
{
    for (int i = 0; i < n; i++) {
        x[i] *= 0.54 - 0.46 * COSTBL[i];
    }
}

void
FFT::hanning(double* x)
{
    for (int i = 0; i < n; i++) {
        x[i] *= 0.5 - 0.5 * COSTBL[i];
    }
}

int
main(int ac, char* av[])
{
    FFT* fft = new FFT(32);
}
