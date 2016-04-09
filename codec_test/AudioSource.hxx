
#include "Codec.hxx"
#include "wav.hxx"
#include <QIODevice>

class AudioSource : public QIODevice {
public:
    AudioSource(void);
    virtual ~AudioSource(void);
    virtual bool atEnd() const;
    virtual qint64 bytesAvailable() const;
    virtual bool canReadLine() const;
    virtual void close(void);
    virtual bool isSequential() const;
    virtual bool open(OpenMode mode);
    //virtual qint64 pos() const { return pos - buf; };
    virtual bool reset();
    virtual bool seek(qint64 pos);
    virtual qint64 size() const;
    virtual bool waitForReadyRead(int);
    virtual qint64 writeData(const char*, qint64);
    void setFileName(const QString& fname) { this->fname = fname; };
    void setCodec(Codec* codec) { this->codec = codec; };
    int getHz(void) const { return fmt.samples_per_sec; };
    WAVFmt::Type getType(void) const { return fmt.type; };
    QString getTypeStr(void) const;
    int getLengthSec(void) const;
    int getBps(void) const { return bps; };
protected:
    qint64 readData(char* data, qint64 maxSize);
private:
    bool openFile(void);
    void clear(void);
    size_t procData(char* dst, size_t len);
    size_t adjustReadLen(qint64 maxSize);
    bool read_linear(WAV& wav, string& errmsg);
    bool read_ulaw(WAV& wav, string& errmsg);
private:
    QString fname;
    WAVFmt fmt;
    char* buf;
    size_t len;
    const char* pos;
    size_t rest;
    int bps;
    Codec* codec;
    char* codec_rest;
    int codec_rest_len;
};
