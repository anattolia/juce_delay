/*
  ==============================================================================

    Filter.h
    Created: 19 Jun 2025 9:51:37am
    Author:  masal

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Filter
{
public:
    Filter();
    ~Filter();

    void prepare(juce::dsp::ProcessSpec& spec);
   // void process(juce::AudioBuffer<float>& buffer, Filter filter);

    void processHPF(juce::AudioBuffer<float>& buffer);
    void processLPF(juce::AudioBuffer<float>& buffer);
    void setHPFFreqValue(float newFreqValue);
    void setLPFFreqValue(float newFreqValue);

    float frequency{ 0.0f };

private:

    // High pass
    juce::dsp::StateVariableTPTFilter<float> hpf;
  
    // Low pass
    juce::dsp::StateVariableTPTFilter<float> lpf;

    float hpfFreq{ 0.0f };
    float lpfFreq{ 20000.0f };

};
