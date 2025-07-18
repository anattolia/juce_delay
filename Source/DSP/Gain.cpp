/*
  ==============================================================================

    Gain.cpp
    Created: 18 Jun 2025 5:10:02pm
    Author:  masal

  ==============================================================================
*/

#include "Gain.h"

Gain::Gain() {};
Gain::~Gain() {};

void Gain::setGainValue(float newGainValue)
{
    gainValue = newGainValue;
}

void Gain::process(juce::AudioBuffer<float>& buffer)
{
    for (int channel = 0; channel < buffer.getNumChannels(); channel++)
    {
        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            auto sample = buffer.getSample(channel, i);
            float outSample = sample * gainValue;
            buffer.setSample(channel, i, outSample);
        }
    }
}