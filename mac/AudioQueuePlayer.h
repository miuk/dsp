//
//  AudioQueuePlayer.h
//  AudioQueue
//
//  Created by Norihisa Nagano
//

#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>

@interface AudioQueuePlayer: NSObject {
    AudioQueueRef audioQueueObject;    //使用するAudio Queueオブジェクト
    AudioFileID   audioFileID;         //再生するサウンドファイル
    UInt32        numPacketsToRead;    //何パケットずつ読み込むか
    SInt64        startingPacketCount; //現在の読み込み位置
    BOOL          donePlayingFile;     //サウンドファイルのすべてを読み終わったか
}

@property UInt32       numPacketsToRead;
@property AudioFileID  audioFileID;
@property SInt64       startingPacketCount;
@property BOOL         donePlayingFile;

-(void)prepareAudioQueue; //Audio Queueの準備用メソッド
-(void)play;              //再生
@end