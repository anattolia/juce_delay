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

void Delay::setFeedbackValue(float newFeedbackValue){
    mFeedback = newFeedbackValue;

}

void Delay::fillDelayBuffer(int channel, const int bufferLength, const int delayBufferLength, const float* bufferData, const float* delayBufferData)
{
    // Calculate read position for feedback
    const int feedbackReadPosition = static_cast<int> (delayBufferLength + mWritePosition - (mSampleRate * delayTimeMsValue / 1000)) % delayBufferLength;

    if (delayBufferLength > bufferLength + mWritePosition) {
        for (int i = 0; i < bufferLength; ++i) {
            float delayedSample = delayBufferData[(feedbackReadPosition + i) % delayBufferLength];
            float inputSample = bufferData[i] + (mFeedback * delayedSample); // Camino de feedback -> filtrar 
            mDelayBuffer.setSample(channel, mWritePosition + i, inputSample * 0.8f); // 0.8f: ramp/gain
        }
    }
    else {
        const int bufferRemaining = delayBufferLength - mWritePosition;
        for (int i = 0; i < bufferRemaining; ++i) {
            float delayedSample = delayBufferData[(feedbackReadPosition + i) % delayBufferLength];
            float inputSample = bufferData[i] + mFeedback * delayedSample;
            mDelayBuffer.setSample(channel, mWritePosition + i, inputSample * 0.8f);
        }
        for (int i = 0; i < bufferLength - bufferRemaining; ++i) {
            float delayedSample = delayBufferData[(i) % delayBufferLength];
            float inputSample = bufferData[bufferRemaining + i] + mFeedback * delayedSample;
            mDelayBuffer.setSample(channel, i, inputSample * 0.8f);
        }
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
    mDelayBuffer.clear(); // <-- Clear buffer to avoid garbage values
    mWritePosition = 0;   // <-- Reset write position
}

void Delay::process(juce::AudioBuffer<float>& buffer, int numInputChannels) {
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
    mWritePosition %= delayBufferLength; // Asegurarse de que no se salga del l�mite
}
