class FFT {
public:
    /// @param n FFT ポイント数. 2 のべき乗である必要がある.
    FFT(int n);
    ~FFT(void);
    double* s2d(const short* src);
    short* d2s(const double* src);
    void fft(double* x, double* y);
    void ifft(double* x, double* y);
    double* abs(const double* x, const double* y, bool bHalf=false);
    double* angle(const double* x, const double* y, bool bHalf=false);
    void hamming(double* x);
    void hanning(double* x);
private:
    int n;
    int* BITREV;
    double* SINTBL;             // for FFT, IFFT
    double* COSTBL;             // for hamming/hanning window
};
