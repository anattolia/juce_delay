/*
  ==============================================================================

    DryWet.cpp
    Created: 17 Jun 2025 10:51:43pm
    Author:  masal

  ==============================================================================
*/

#include "DryWet.h"

DryWet::DryWet() {};
DryWet::~DryWet() {};

void DryWet::setDryWetValue(float newDryWetValue)
{
    dryWetValue = newDryWetValue;
}

void DryWet::process(juce::AudioBuffer<float>& dryBuffer, juce::AudioBuffer<float>& wetBuffer)
{
    for (int channel = 0; channel < dryBuffer.getNumChannels(); channel++)
    {
        for (int i = 0; i < dryBuffer.getNumSamples(); i++)
        {
            float drySample = dryBuffer.getSample(channel, i);
            float wetSample = wetBuffer.getSample(channel, i);

            float outSample = (drySample * (1 - dryWetValue)) + (wetSample * dryWetValue);

            wetBuffer.setSample(channel, i, outSample);
        }
    }
}


