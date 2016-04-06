#include "CodecWrapper.hxx"
#include "NullCodec.hxx"
#include "SpeexCodec.hxx"

using namespace std;

CodecWrapper::CodecWrapper(void)
{
    Codec* codec = new NullCodec();
    codec->init();
    codecs.push_back(codec);
    setCodec(0);
}

CodecWrapper::~CodecWrapper(void)
{
    for (vector<Codec*>::iterator i = codecs.begin();
         i != codecs.end(); i++) {
        Codec* codec = *i;
        delete codec;
    }
    codecs.clear();
}

const char*
CodecWrapper::getName(void) const
{
    static const char* name = "wrapper";
    return name;
}

void
CodecWrapper::addCodec(Codec* codec)
{
    codec->init();
    codecs.push_back(codec);
}

void
CodecWrapper::setCodec(int num)
{
    curCodec = codecs[num];
    curCodecNum = num;
}

const char*
CodecWrapper::getCodecName(int num) const
{
    return codecs[num]->getName();
}

void
CodecWrapper::init(void)
{
    curCodec->init();
}

void
CodecWrapper::clear(void)
{
    curCodec->clear();
}

int
CodecWrapper::getFrameSize(void)
{
    return curCodec->getFrameSize();
}

int
CodecWrapper::encode(const int16_t* src, int srclen, char* dst)
{
    return curCodec->encode(src, srclen, dst);
}

int
CodecWrapper::decode(const char* src, int srclen, int16_t* dst)
{
    return curCodec->decode(src, srclen, dst);
}

int
CodecWrapper::codec(const int16_t* src, int srclen, int16_t* dst)
{
    return curCodec->codec(src, srclen, dst);
}
