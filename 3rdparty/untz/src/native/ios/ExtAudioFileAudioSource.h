//
//  ExtAudioFileAudioSource.h
//  Part of UNTZ
//
//  Created by Zach Saul (zach@retronyms.com) on 06/01/2011.
//  Copyright 2011 Retronyms. All rights reserved.
//

#ifndef EXTAUDIOFILEAUDIOSOURCE_H_
#define EXTAUDIOFILEAUDIOSOURCE_H_

#include "BufferedAudioSource.h"
#include <AudioToolbox/AudioToolbox.h>

class ExtAudioFileAudioSource : public BufferedAudioSource
{
public:
	ExtAudioFileAudioSource();
    ~ExtAudioFileAudioSource();

	// AudioSource
	double getSampleRate();
	double getLength();
	UInt32 getNumChannels();
    
    // BufferedAudioSource
	virtual bool init(const RString& path, bool loadIntoMemory);
    virtual void close();
	virtual Int64 decodeData(float* buffer, UInt32 size);
	virtual void setDecoderPosition(Int64 startFrame);
	virtual void doneDecoding();
    
protected:
    void setUpBuffers(float *buffer, UInt32 numChannels, UInt32 numFrames);

    ExtAudioFileRef mAudioFile;
    AudioStreamBasicDescription mClientFormat;
    AudioStreamBasicDescription mFormat;
    AudioBufferList *mpBufferList;
    SInt64 mTotalFrames;
    std::vector<float>mReadBuffer;
};

#endif