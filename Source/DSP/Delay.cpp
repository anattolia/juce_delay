/*
  ==============================================================================

    Delay.cpp
    Created: 12 Jun 2025 7:52:52pm
    Author:  masal

  ==============================================================================
*/

#include "Delay.h"

Delay::Delay() {};
Delay::~Delay() {};

void Delay::setDelayTimeValue(float newDelayTimeValue) {
    delayTimeMsValue = newDelayTimeValue;

}

void Delay::fillDelayBuffer(int channel, const int bufferLength, const int delayBufferLength, const float* bufferData, const float* delayBufferData)
{
    // Copiar los datos del buffer al delay buffer
    if (delayBufferLength > bufferLength + mWritePosition) {
        mDelayBuffer.copyFromWithRamp(channel, mWritePosition, bufferData, bufferLength, 0.8, 0.8);
    }
    else {
        const int bufferRemaining = delayBufferLength - mWritePosition;

        mDelayBuffer.copyFromWithRamp(channel, mWritePosition, bufferData, bufferRemaining, 0.8, 0.8);
        mDelayBuffer.copyFromWithRamp(channel, 0, bufferData, bufferLength - bufferRemaining, 0.8, 0.8);
    }
}

void Delay::getFromDelayBuffer(juce::AudioBuffer<float>& buffer, int channel, const int bufferLength, const int delayBufferLength, const float* bufferData, const float* delayBufferData)
{
    const int readPosition = static_cast<int> (delayBufferLength + mWritePosition - (mSampleRate * delayTimeMsValue / 1000)) % delayBufferLength;

    if (delayBufferLength > bufferLength + readPosition) {
        buffer.addFrom(channel, 0, delayBufferData + readPosition, bufferLength);
    }
    else {
        const int bufferRemaining = delayBufferLength - readPosition;
        buffer.addFrom(channel, 0, delayBufferData + readPosition, bufferRemaining);
        buffer.addFrom(channel, bufferRemaining, delayBufferData, bufferLength - bufferRemaining);
    }
}

void Delay::prepare(double sampleRate, int samplesPerBlock)
{
	const int numInputChannels = 2; // Assuming stereo buffer
	const int delayBufferSize = 5 * (sampleRate + samplesPerBlock); // the number of seconds available for the delay
	mSampleRate = sampleRate;
	mDelayBuffer.setSize(numInputChannels, delayBufferSize);
}

void Delay::process(juce::AudioBuffer<float>& buffer, int delayTimeMs, int numInputChannels) {
    const int bufferLength = buffer.getNumSamples();
    const int delayBufferLength = mDelayBuffer.getNumSamples();

    for (int channel = 0; channel < numInputChannels; ++channel)
    {
        const float* bufferData = buffer.getReadPointer(channel);
        const float* delayBufferData = mDelayBuffer.getReadPointer(channel);

        fillDelayBuffer(channel, bufferLength, delayBufferLength, bufferData, delayBufferData);
        getFromDelayBuffer(buffer, channel, bufferLength, delayBufferLength, bufferData, delayBufferData);
    }

    mWritePosition += bufferLength;
    mWritePosition %= delayBufferLength; // Asegurarse de que no se salga del límite
}
