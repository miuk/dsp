#include <CoreFoundation/CoreFoundation.h>
#include <CoreAudio/CoreAudio.h>
#include <stdio.h>
#include <sys/time.h>
#include <math.h>

class DirectAudio {
public:
    DirectAudio(void) {
        nRecordCalled = 0;
        nPlayCalled = 0;
        nRecordBytes = 0;
        nPlayBytes = 0;
    };
    bool setUp(void);
    void stop(void);
    static OSStatus recordCB(AudioDeviceID devId
                             , const AudioTimeStamp* now
                             , const AudioBufferList* inData
                             , const AudioTimeStamp* inTime
                             , AudioBufferList* outData
                             , const AudioTimeStamp* outTime
                             , void* clientData);
    static OSStatus playCB(AudioDeviceID devId
                           , const AudioTimeStamp* now
                           , const AudioBufferList* inData
                           , const AudioTimeStamp* inTime
                           , AudioBufferList* outData
                           , const AudioTimeStamp* outTime
                           , void* clientData);

    void onRecord(const AudioTimeStamp* now
                  , const AudioBufferList* inData
                  , const AudioTimeStamp* inTime);

    void onPlay(const AudioTimeStamp* now
                , AudioBufferList* outData
                , const AudioTimeStamp* outTime);

    int getNRecord(void) const { return nRecordCalled; };
    int getNPlay(void) const { return nPlayCalled; };
    int getNRecordBytes(void) const { return nRecordBytes; };
    int getNPlayBytes(void) const { return nPlayBytes; };

    static void listDevice(void);
    static void showDeviceInfo(const AudioDeviceID& devId);

private:
    void fillWave(void);

private:
    AudioDeviceID recordDevId;
    AudioDeviceID playDevId;
    AudioDeviceIOProcID recordProcId;
    AudioDeviceIOProcID playProcId;
    int nRecordCalled;
    int nPlayCalled;
    int nRecordBytes;
    int nPlayBytes;

    float playWave[512];
};

void
DirectAudio::fillWave(void)
{
    float w = 2.0 * M_PI / (float)512 * 2;
    for (int i = 0; i < 512; i++) {
        playWave[i] = sin(w * (float)i);
    }
}

OSStatus
DirectAudio::recordCB(AudioDeviceID devId
                      , const AudioTimeStamp* now
                      , const AudioBufferList* inData
                      , const AudioTimeStamp* inTime
                      , AudioBufferList* outData
                      , const AudioTimeStamp* outTime
                      , void* clientData)
{
    DirectAudio* audio = (DirectAudio*)clientData;
    audio->onRecord(now, inData, inTime);
    return noErr;
}

OSStatus
DirectAudio::playCB(AudioDeviceID devId
                    , const AudioTimeStamp* now
                    , const AudioBufferList* inData
                    , const AudioTimeStamp* inTime
                    , AudioBufferList* outData
                    , const AudioTimeStamp* outTime
                    , void* clientData)
{
    DirectAudio* audio = (DirectAudio*)clientData;
    audio->onPlay(now, outData, outTime);
    return noErr;
}

void
DirectAudio::onRecord(const AudioTimeStamp* now
                      , const AudioBufferList* inData
                      , const AudioTimeStamp* inTime)
{
    UInt32 nBuf = inData->mNumberBuffers;
    if (nBuf <= 0)
        return;
    UInt32 nByte = inData->mBuffers[0].mDataByteSize;
    float* buf = (float*)inData->mBuffers[0].mData;
    int nPos = nByte / sizeof(float);
    float total = 0;
    for (int i = 0; i < nPos; i++) {
        total += fabs(buf[i]);
    }
/*
    float avg = total / (float)nPos;
    if (avg > 0.01)
        printf("%f\n", avg);
*/
//    UInt32 outBuffers = outData->mNumberBuffers;
//    UInt32 nbyte = outData->mBuffers[0].mDataByteSize;
//    printf("in buffers=%d, bytes=%d\n", inBuffers, nbyte);
    nRecordCalled++;
    nRecordBytes += nByte;
/*
    struct timeval tv;
    gettimeofday(&tv, NULL);
    printf("%ld.%06d nByte=%d, cnt=%d\n",
           tv.tv_sec, tv.tv_usec,
           nByte, nRecordCalled);
*/
}

void
DirectAudio::onPlay(const AudioTimeStamp* now
                    , AudioBufferList* outData
                    , const AudioTimeStamp* outTime)
{
    UInt32 nBuf = outData->mNumberBuffers;
    if (nBuf <= 0)
        return;
    UInt32 nByte = outData->mBuffers[0].mDataByteSize;
    float* buf = (float*)outData->mBuffers[0].mData;
    int nPos = nByte / 2 / sizeof(float);
    if (nPos > 512)
        nPos = 512;
    memset(buf, 0, nByte);

    for (int i = 0; i < nPos; i++) {
        buf[i*2+1] = playWave[i];
    }

//    UInt32 outBuffers = outData->mNumberBuffers;
//    UInt32 nbyte = outData->mBuffers[0].mDataByteSize;
//    printf("out buffers=%d, bytes=%d\n", outBuffers, nbyte);
    nPlayCalled++;
    nPlayBytes += nByte;
}

static void
show(const char* str, const AudioStreamBasicDescription& stream_desc)
{
    printf(" %s\n", str);
    printf("  mSampleRate=%f\n", stream_desc.mSampleRate);
    printf("  mFormatID=%x\n", stream_desc.mFormatID);
    printf("  mFormatFlags=%d\n", stream_desc.mFormatFlags);
    printf("  mBytesPerPacket=%d\n", stream_desc.mBytesPerPacket);
    printf("  mFramesPerPacket=%d\n", stream_desc.mFramesPerPacket);
    printf("  mBytesPerFrame=%d\n", stream_desc.mBytesPerFrame);
    printf("  mChannelsPerFrame=%d\n", stream_desc.mChannelsPerFrame);
    printf("  mBitsPerChannel=%d\n", stream_desc.mBitsPerChannel);
}

bool
DirectAudio::setUp(void)
{
    fillWave();

    // get default input device
    UInt32 size = sizeof(recordDevId);
    AudioObjectPropertyAddress addr;
    addr.mSelector = kAudioHardwarePropertyDefaultInputDevice;
    addr.mScope = kAudioObjectPropertyScopeGlobal;
    addr.mElement = kAudioObjectPropertyElementMaster;
    OSStatus err = AudioObjectGetPropertyData(kAudioObjectSystemObject
                                              , &addr
                                              , 0
                                              , NULL
                                              , &size
                                              , &recordDevId);
    if (err != noErr) {
        return false;
    }
    printf("recordDevId=%d, size=%d\n", recordDevId, size);

    // get default output device
    size = sizeof(playDevId);
    addr.mSelector = kAudioHardwarePropertyDefaultOutputDevice;
    addr.mScope = kAudioObjectPropertyScopeGlobal;
    addr.mElement = kAudioObjectPropertyElementMaster;
    err = AudioObjectGetPropertyData(kAudioObjectSystemObject
                                     , &addr
                                     , 0
                                     , NULL
                                     , &size
                                     , &playDevId);
    if (err != noErr) {
        return false;
    }
    printf("playDevId=%d, size=%d\n", playDevId, size);

    AudioStreamBasicDescription stream_desc;
    size = sizeof(stream_desc);
    addr.mSelector = kAudioDevicePropertyStreamFormat;
    addr.mScope = kAudioObjectPropertyScopeInput;
    addr.mElement = 1;
    err = AudioObjectGetPropertyData(recordDevId
                                     , &addr
                                     , 0
                                     , NULL
                                     , &size
                                     , &stream_desc);
    if (err != noErr) {
        return false;
    }
    show("record", stream_desc);

    size = sizeof(stream_desc);
    addr.mSelector = kAudioDevicePropertyStreamFormat;
    addr.mScope = kAudioObjectPropertyScopeOutput;
    addr.mElement = 1;
    err = AudioObjectGetPropertyData(playDevId
                                     , &addr
                                     , 0
                                     , NULL
                                     , &size
                                     , &stream_desc);
    if (err != noErr) {
        return false;
    }
    show("play", stream_desc);

    err = AudioDeviceCreateIOProcID(recordDevId
                                    , DirectAudio::recordCB
                                    , this
                                    , &recordProcId);
    if (err != noErr) {
        return false;
    }
    err = AudioDeviceCreateIOProcID(playDevId
                                    , DirectAudio::playCB
                                    , this
                                    , &playProcId);
    if (err != noErr) {
        return false;
    }

    err = AudioDeviceStart(recordDevId, recordProcId);
    if (err != noErr) {
        return false;
    }
    err = AudioDeviceStart(playDevId, playProcId);
    if (err != noErr) {
        return false;
    }
    return true;
}

void
DirectAudio::stop(void)
{
    AudioDeviceStop(recordDevId, recordProcId);
    AudioDeviceStop(playDevId, playProcId);
    AudioDeviceDestroyIOProcID(recordDevId, recordProcId);
    AudioDeviceDestroyIOProcID(playDevId, playProcId);
}

void
DirectAudio::listDevice(void)
{
    OSStatus result = noErr;
    // get the device list	
    AudioObjectPropertyAddress props = {
        kAudioHardwarePropertyDevices,
        kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMaster
    };
    UInt32 propSize;
    result = AudioObjectGetPropertyDataSize(kAudioObjectSystemObject,
                                            &props,
                                            0,
                                            NULL,
                                            &propSize);
    if (result != noErr) {
        printf("Error in AudioObjectGetPropertyDataSize: %d\n", result);
        return;
    }
		
    // Find out how many devices are on the system
    UInt32 nDevice = 0;
    nDevice = propSize / sizeof(AudioDeviceID);
    printf("propSize=%u, nDevice=%u\n", propSize, nDevice);
    AudioDeviceID devices[nDevice];
    result = AudioObjectGetPropertyData(kAudioObjectSystemObject,
                                        &props,
                                        0,
                                        NULL,
                                        &propSize,
                                        devices);
    if (result != noErr) {
        printf("Error in AudioObjectGetPropertyData: %d\n", result);
        return;
    }

    CFStringRef name;
    for (UInt32 i=0; i < nDevice; i++) {
        showDeviceInfo(devices[i]);
    }
}

void
DirectAudio::showDeviceInfo(const AudioDeviceID& devId)
{
    // get the device name
    OSStatus result = noErr;
    CFStringRef name;
    AudioObjectPropertyAddress props = {
        kAudioObjectPropertyName,
        kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMaster
    };
    UInt32 propSize = sizeof(CFStringRef);
    result = AudioObjectGetPropertyData(devId,
                                        &props,
                                        0,
                                        NULL,
                                        &propSize,
                                        &name);
    if (result != noErr) {
        printf("Error in AudioObjectGetPropertyData: %d\n", result);
        return;
    }
    printf("device id=%d, name=%s\n", devId,
           CFStringGetCStringPtr(name, kCFStringEncodingMacRoman));

    // get record info
    AudioStreamBasicDescription stream_desc;
    propSize = sizeof(stream_desc);
    props.mSelector = kAudioDevicePropertyStreamFormat;
    props.mScope = kAudioObjectPropertyScopeInput;
    props.mElement = 1;
    result = AudioObjectGetPropertyData(devId,
                                        &props,
                                        0, 
                                        NULL,
                                        &propSize,
                                        &stream_desc);
    if (result == noErr) {
        show("record", stream_desc);
    }

    // get play info
    propSize = sizeof(stream_desc);
    props.mSelector = kAudioDevicePropertyStreamFormat;
    props.mScope = kAudioObjectPropertyScopeOutput;
    props.mElement = 1;
    result = AudioObjectGetPropertyData(devId,
                                        &props,
                                        0, 
                                        NULL,
                                        &propSize,
                                        &stream_desc);
    if (result == noErr) {
        show("play", stream_desc);
    }
}

static void
iotest(void)
{
    DirectAudio audio;
    if (!audio.setUp()) {
        printf("setup failed\n");
        exit(1);
    }
    struct timeval begin;
    gettimeofday(&begin, NULL);
    getchar();
    audio.stop();
    printf("nrec=%d, nplay=%d\n", audio.getNRecord(), audio.getNPlay());
    printf("recBytes=%d, playBytes=%d\n",
           audio.getNRecordBytes(), audio.getNPlayBytes());
    struct timeval end;
    gettimeofday(&end, NULL);
    struct timeval diff;
    diff.tv_sec = end.tv_sec - begin.tv_sec;
    diff.tv_usec = end.tv_usec - begin.tv_usec;
    if (diff.tv_usec < 0) {
        diff.tv_sec--;
        diff.tv_usec += 1000000;
    }
    printf("%ld.%06d\n", diff.tv_sec, diff.tv_usec);
    long us = diff.tv_sec * 1000000 + diff.tv_usec;
    long interval = us / audio.getNRecord();
    printf("interval=%ld us\n", interval);
}


int
main(int ac, char* av[])
{
    if (ac > 1 && !strcmp(av[1], "--list")) {
        DirectAudio::listDevice();
    } else {
        iotest();
    }
}
