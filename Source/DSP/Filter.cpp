/*
  ==============================================================================

    Filter.cpp
    Created: 19 Jun 2025 9:51:37am
    Author:  masal

  ==============================================================================
*/

#include "Filter.h"
#include "LFO.h"

Filter::Filter() {};
Filter::~Filter() {};

void Filter::prepare(juce::dsp::ProcessSpec& spec)
{
    // high Pass Filter
    hpf.prepare(spec);
    hpf.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    hpf.setCutoffFrequency(20.0f);

    // Low Pass Filter
    lpf.prepare(spec);
    lpf.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    lpf.setCutoffFrequency(10000.0f);
}

void Filter::processHPF(juce::AudioBuffer<float>& buffer)
{
    // Preguntar la sintaxis de esto qué
    auto audioBlock = juce::dsp::AudioBlock<float>(buffer);
    auto context = juce::dsp::ProcessContextReplacing<float>(audioBlock);

    hpf.setCutoffFrequency(hpfFreq);
    hpf.process(context);
}

void Filter::processLPF(juce::AudioBuffer<float>& buffer) {
    auto audioBlock = juce::dsp::AudioBlock<float>(buffer);
    auto context = juce::dsp::ProcessContextReplacing<float>(audioBlock);

    lpf.setCutoffFrequency(lpfFreq);
    lpf.process(context);
}

void Filter::setHPFFreqValue(float newFreqValue)
{
    hpfFreq = newFreqValue;
}

void Filter::setLPFFreqValue(float newFreqValue)
{
    lpfFreq = newFreqValue;
}