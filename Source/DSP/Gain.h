/*
  ==============================================================================

    Gain.h
    Created: 18 Jun 2025 5:10:02pm
    Author:  masal

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Gain
{
public:
    Gain();
    ~Gain();

    void setGainValue(float newGainValue);
    void process(juce::AudioBuffer<float>& buffer);
    void prepare(double sampleRate);

private:
    juce::LinearSmoothedValue<float> gainValue{ 1.0f };
};