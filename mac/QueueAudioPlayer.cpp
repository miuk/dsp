#include <CoreFoundation/CoreFoundation.h>
#include <CoreAudio/CoreAudio.h>
#include <AudioToolbox/AudioQueue.h>
#include <stdio.h>
#include <sys/time.h>
#include <math.h>

static const int kNumberBuffers = 3;
static const int kFramesPerBuffer = 65536;

class QueueAudioPlayer {
public:

    bool init(void);
    bool start(void);
    bool setGain(float gain);

    void audioQueueOutputWithAudioQueue(AudioQueueRef inAQ,
                                        AudioQueueBufferRef inBuffer);

    static void audioQueueOutputCallback(void* inUserData,
                                         AudioQueueRef inAQ,
                                         AudioQueueBufferRef inBuffer);

private:
    AudioStreamBasicDescription stream_desc;
    int _frameIndex;
    AudioQueueRef audioQueue;
    AudioQueueBufferRef buffers[kNumberBuffers];
};

void
QueueAudioPlayer::audioQueueOutputCallback(void* inUserData,
                                     AudioQueueRef inAQ,
                                     AudioQueueBufferRef inBuffer)
{
    QueueAudioPlayer* qa = (QueueAudioPlayer*)inUserData;
    qa->audioQueueOutputWithAudioQueue(inAQ, inBuffer);
}

void
QueueAudioPlayer::audioQueueOutputWithAudioQueue(AudioQueueRef inAQ,
                                           AudioQueueBufferRef inBuffer)
{
    printf("callback\n");
    inBuffer->mAudioDataByteSize
        = stream_desc.mBytesPerFrame * kFramesPerBuffer;
    float freq = 440.0f;
    float phasePerSample = freq / stream_desc.mSampleRate;
    float* sampleBuffer = (float*)inBuffer->mAudioData;
    for (int i = 0; i < kFramesPerBuffer; i++) {
        float wt = (float)_frameIndex * phasePerSample * M_PI * 2.0;
        sampleBuffer[i*2] = sinf(wt);
        sampleBuffer[i*2+1] = sinf(wt);
        _frameIndex++;
    }
    AudioQueueEnqueueBuffer(inAQ, inBuffer, 0, NULL);
}

bool
QueueAudioPlayer::init(void)
{
    stream_desc.mSampleRate = 44100;
    stream_desc.mFormatID = kAudioFormatLinearPCM;
    stream_desc.mFormatFlags = kLinearPCMFormatFlagIsFloat | kAudioFormatFlagIsPacked;
    stream_desc.mBytesPerPacket = 8;
    stream_desc.mFramesPerPacket = 1;
    stream_desc.mBytesPerFrame = 8;
    stream_desc.mChannelsPerFrame = 2;
    stream_desc.mBitsPerChannel = 32;

    OSStatus err = AudioQueueNewOutput(&stream_desc,
                                       audioQueueOutputCallback,
                                       this,
                                       CFRunLoopGetCurrent(),
                                       kCFRunLoopCommonModes,
                                       0,
                                       &audioQueue);
    if (err != noErr) {
        fprintf(stderr, "AudioQueueNewOutput failed. %d\n", err);
        return false;
    }

    UInt32 bufferSize = kFramesPerBuffer * stream_desc.mBytesPerFrame;
    for (int i = 0; i < kNumberBuffers; i++) {
        err = AudioQueueAllocateBuffer(audioQueue, bufferSize, &buffers[i]);
        if (err != noErr)
            fprintf(stderr, "AudioQueueAllocateBuffer failed. %d\n", err);
        audioQueueOutputWithAudioQueue(audioQueue, buffers[i]);
    }

    return true;
}

bool
QueueAudioPlayer::setGain(float gain)
{
    OSStatus err = AudioQueueSetParameter(audioQueue, kAudioQueueParam_Volume, gain);
    return (err != noErr);
}

bool
QueueAudioPlayer::start(void)
{
    OSStatus err = AudioQueueStart(audioQueue, NULL);
    return (err != noErr);
}

/*
int
main(int ac, char* av[])
{
    QueueAudioPlayer qa;
    qa.init();
    qa.setGain(0.1);
    qa.start();

    for (;;) {
        fprintf(stderr, "loop\n");
        CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0.25, false);
    }

}
*/
