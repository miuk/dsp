#include <CoreFoundation/CoreFoundation.h>
#include <CoreAudio/CoreAudio.h>
#include <AudioToolbox/AudioQueue.h>
#include <stdio.h>
#include <sys/time.h>
#include <math.h>

static const int kNumberBuffers = 3;
//static const int kFramesPerBuffer = 65536;
static const int kFramesPerBuffer = 160;

class QueueAudioRecorder {
public:
    QueueAudioRecorder(void) {
        nRecPackets = 0;
    };

    bool init(void);
    bool start(void);
    bool setGain(float gain);
    bool stop(void);
    bool clear(void);

    void audioQueueInputWithAudioQueue(AudioQueueRef inAQ,
                                       AudioQueueBufferRef inBuffer,
                                       const AudioTimeStamp* inStartTime,
                                       UInt32 inNumPackets,
                                       const AudioStreamPacketDescription* inPacketDesc);

    static void audioQueueInputCallback(void* inUserData,
                                        AudioQueueRef inAQ,
                                        AudioQueueBufferRef inBuffer,
                                        const AudioTimeStamp* inStartTime,
                                        UInt32 inNumPackets,
                                        const AudioStreamPacketDescription* inPacketDesc);

private:
    AudioStreamBasicDescription stream_desc;
    int _frameIndex;
    AudioQueueRef audioQueue;
    AudioQueueBufferRef buffers[kNumberBuffers];
    int nRecPackets;
};

void
QueueAudioRecorder::audioQueueInputCallback(void* inUserData,
                                            AudioQueueRef inAQ,
                                            AudioQueueBufferRef inBuffer,
                                            const AudioTimeStamp* inStartTime,
                                            UInt32 inNumPackets,
                                            const AudioStreamPacketDescription* inPacketDesc)
{
    QueueAudioRecorder* qa = (QueueAudioRecorder*)inUserData;
    qa->audioQueueInputWithAudioQueue(inAQ, inBuffer, inStartTime,
                                      inNumPackets, inPacketDesc);
}

void
QueueAudioRecorder::audioQueueInputWithAudioQueue(AudioQueueRef inAQ,
                                                  AudioQueueBufferRef inBuffer,
                                                  const AudioTimeStamp* inStartTime,
                                                  UInt32 inNumPackets,
                                                  const AudioStreamPacketDescription* inPacketDesc)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    nRecPackets += inNumPackets;
    printf("callback %ld.%06d %d, %d, %d\n",
           tv.tv_sec, tv.tv_usec,
           inNumPackets, inBuffer->mAudioDataByteSize, nRecPackets);
    AudioQueueEnqueueBuffer(inAQ, inBuffer, 0, NULL);
}

bool
QueueAudioRecorder::init(void)
{
//    stream_desc.mSampleRate = 44100;
    stream_desc.mSampleRate = 8000;
    stream_desc.mFormatID = kAudioFormatLinearPCM;
    //stream_desc.mFormatFlags = kLinearPCMFormatFlagIsFloat | kAudioFormatFlagIsPacked;
    stream_desc.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    stream_desc.mBytesPerPacket = 2;
    stream_desc.mFramesPerPacket = 1;
    stream_desc.mBytesPerFrame = 2;
    stream_desc.mChannelsPerFrame = 1;
    stream_desc.mBitsPerChannel = 16;

    OSStatus err = AudioQueueNewInput(&stream_desc,
                                      audioQueueInputCallback,
                                      this,
                                      NULL,
                                      kCFRunLoopCommonModes,
                                      0,
                                      &audioQueue);
    if (err != noErr) {
        fprintf(stderr, "AudioQueueNewInput failed. %d\n", err);
        return false;
    }

    UInt32 bufferSize = kFramesPerBuffer * stream_desc.mBytesPerFrame;
    for (int i = 0; i < kNumberBuffers; i++) {
        err = AudioQueueAllocateBuffer(audioQueue, bufferSize, &buffers[i]);
        if (err != noErr)
            fprintf(stderr, "AudioQueueAllocateBuffer failed. %d\n", err);
        //audioQueueInputWithAudioQueue(audioQueue, buffers[i]);
        AudioQueueEnqueueBuffer(audioQueue, buffers[i], 0, NULL);
    }

    return true;
}

bool
QueueAudioRecorder::setGain(float gain)
{
    OSStatus err = AudioQueueSetParameter(audioQueue, kAudioQueueParam_Volume, gain);
    return (err != noErr);
}

bool
QueueAudioRecorder::start(void)
{
    OSStatus err = AudioQueueStart(audioQueue, NULL);
    return (err != noErr);
}

bool
QueueAudioRecorder::stop(void)
{
    OSStatus err = AudioQueueStop(audioQueue, true);
    return (err != noErr);
}

bool
QueueAudioRecorder::clear(void)
{
    OSStatus err = AudioQueueDispose(audioQueue, true);
    return (err != noErr);
}

#ifdef notdef
int
main(int ac, char* av[])
{
    QueueAudioRecorder qa;
    qa.init();
    qa.setGain(0.1);
    qa.start();
    getchar();
    qa.stop();
    getchar();
    qa.clear();
/*
    for (;;) {
        fprintf(stderr, "loop\n");
        CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0.25, false);
    }
*/
}
#endif
