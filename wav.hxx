#include <string>
using namespace std;

// data of 'fmt' chunk
class WAVFmt {
public:

    enum Type {
        Type_unknown = 0x0000,
        Type_pcm = 0x0001,
        Type_mulaw = 0x0007,
        Type_GSM = 0x0031,
    };

public:
    WAVFmt(void);
    WAVFmt(Type type, int nch, int samples_per_sec, int bits_per_sample);
    WAVFmt(const WAVFmt& src) { *this = src; };
    WAVFmt& operator=(const WAVFmt& src);

    bool read(int fd, string& errmsg);
    bool write(int fd, string& errmsg) const;

    int calcBytes(int nsample) const;
    int calcSamples(int nbyte) const;

public:
    Type type;
    int nch;
    int samples_per_sec;
    int bytes_per_sec;
    int block_size;
    int bits_per_sample;
};

class WAV {
public:
    WAV(void);
    // open for read
    bool open(const string& fname, string& errmsg);
    // open for write
    bool open(const string& fname, const WAVFmt& fmt, string& errmsg);
    bool close(string& errmsg);
    int read(char* buf, int nsample);
    int write(const char* buf, int nsample);
    int getDataBytes(void) const { return bytes_data; };
    const WAVFmt& getFmt(void) const { return fmt; };

private:
    bool seek_to(const string& chunk_name);
    bool read_length(unsigned int& len);

private:
    WAVFmt fmt;
    int bytes_data;
    int fd;
    bool bWrite;
};
