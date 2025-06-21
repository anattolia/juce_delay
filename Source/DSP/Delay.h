/*
  ==============================================================================

    Delay.h
    Created: 12 Jun 2025 7:52:52pm
    Author:  masal

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h> 

class Delay
{
public:
	Delay();
	~Delay();

	void setDelayTimeValue(float newDelayTimeValue);
	void setFeedbackValue(float newFeedbackValue);

	void prepare(double sampleRate, int samplesPerBlock);

	void process(juce::AudioBuffer<float>& buffer, int numInputChannels);

	void fillDelayBuffer(int channel, const int bufferLength, const int delayBufferLength, const float* bufferData, const float* delayBufferData);
	void getFromDelayBuffer(juce::AudioBuffer<float>& buffer, int channel, const int bufferLength, const int delatyBufferLength, const float* bufferData, const float* delayBufferData);


private:
	juce::AudioBuffer<float> mDelayBuffer;
	int mWritePosition{ 0 };
	int mSampleRate{ 44100 };

	int delayTimeMsValue;
	float mFeedback{ 0.7f };

};
