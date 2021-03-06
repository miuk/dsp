#include "CodecWrapper.hxx"
#include "NullCodec.hxx"
#include "SpeexCodec.hxx"

using namespace std;

CodecWrapper::CodecWrapper(PacketLossSimulator* pls)
{
    this->pls = pls;
    Codec* codec = new NullCodec();
    codec->init();
    codec->setPLS(pls);
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
    codec->setPLS(pls);
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

void
CodecWrapper::setHz(int hz)
{
    for (vector<Codec*>::iterator i = codecs.begin();
         i != codecs.end(); i++) {
        Codec* codec = *i;
        codec->setHz(hz);
    }
}

void
CodecWrapper::setWAVFmtType(int type)
{
    for (vector<Codec*>::iterator i = codecs.begin();
         i != codecs.end(); i++) {
        Codec* codec = *i;
        codec->setWAVFmtType(type);
    }
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
CodecWrapper::codec(const int16_t* src, int srclen, int16_t* dst, int& bps)
{
    return curCodec->codec(src, srclen, dst, bps);
}
