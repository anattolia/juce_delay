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
#include "DSP/SyncTimeHandler.h"
#include "DSP/LFO.h"

//==============================================================================
/**
* Punto de partida: Juce Tutorial 40- Building a Delay Plugin Pt 1 (Creating a Circular Buffer)
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

    // Preset management
    void savePreset(const juce::String& presetName);
    void loadPreset(const juce::String& presetName);
    juce::StringArray getPresetList();
    juce::File getPresetDirectory();

    // Seguimiento del preset actual
    juce::String getCurrentPresetName() const { return currentPresetName; }
    void setCurrentPresetName(const juce::String& name) { currentPresetName = name; }

    // Inicializar los presets por defecto
    void createDefaultPresets();

private:
    Delay delay;
    DryWet dryWet;
    Gain inputGain;
    Gain outputGain;
    Filter hpf;
    Filter lpf;
	SyncTimeHandler syncTimeHandler;
    LFO lfo;
    
    juce::AudioBuffer<float> dryBuffer;

    bool lfoActive{ false };

    // Variables del preset management
    juce::String currentPresetName = "Default";

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MiauDelay)
};
