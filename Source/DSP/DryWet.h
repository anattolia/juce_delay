/*
  ==============================================================================

    DryWet.h
    Created: 17 Jun 2025 10:51:43pm
    Author:  masal

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class DryWet
{
public:
    DryWet();
    ~DryWet();

    void setDryWetValue(float newDryWetValue);

    void process(juce::AudioBuffer<float>& dryBuffer, juce::AudioBuffer<float>& wetBuffer);

private:
    float dryWetValue{ 0.5f };
};