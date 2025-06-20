/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DSP/Delay.h"
#include "DSP/DryWet.h"
#include "DSP/Gain.h"
#include "DSP/Filter.h"

//==============================================================================
/**
* Tomado de: Juce Tutorial 40- Building a Delay Plugin Pt 1 (Creating a Circular Buffer)
https://youtu.be/IRFUYGkMV8w?si=1tu4UudHqNJSYpe2
*/
class MiauDelay  : public juce::AudioProcessor
{
public:
    //==============================================================================
    MiauDelay();
    ~MiauDelay() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;
	juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

    void updateParameters();
    
private:
    Delay delay;
    DryWet dryWet;
    Gain inputGain;
    Gain outputGain;
    Filter hpf;
    Filter lpf;
    
    juce::AudioBuffer<float> dryBuffer;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MiauDelay)
};
