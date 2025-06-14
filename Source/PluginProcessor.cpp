/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MiauDelay::MiauDelay()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", createParameters())
#endif
{
}

MiauDelay::~MiauDelay()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout MiauDelay::createParameters()
{
    juce::AudioProcessorValueTreeState::ParameterLayout parameters;
    

    // Par�metros del tiempo del delay
    parameters.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{ "DelayTime", 1 }, "DelayTime", 0, 5000, 500));

    return parameters;
}

//==============================================================================
const juce::String MiauDelay::getName() const
{
    return JucePlugin_Name;
}

bool MiauDelay::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MiauDelay::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MiauDelay::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MiauDelay::getTailLengthSeconds() const
{
    return 0.0;
}

int MiauDelay::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MiauDelay::getCurrentProgram()
{
    return 0;
}

void MiauDelay::setCurrentProgram (int index)
{
}

const juce::String MiauDelay::getProgramName (int index)
{
    return {};
}

void MiauDelay::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MiauDelay::prepareToPlay (double sampleRate, int samplesPerBlock)
{
  /* // Hacer un nuevo buffer
	const int numInputChannels = getTotalNumInputChannels();
    const int delayBufferSize = 2 * (sampleRate + samplesPerBlock); // el n�mero son los segundos disponibles para el delay
    mSampleRate = sampleRate;

	mDelayBuffer.setSize(numInputChannels, delayBufferSize); // Assuming stereo buffer
    */

	delay.prepare(sampleRate, samplesPerBlock);
}

void MiauDelay::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MiauDelay::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void MiauDelay::updateParameters()
{
    float inDelayTime = *apvts.getRawParameterValue("DelayTime");
    
   // delayTime.setDelayTimeValue(inDelayTime);
}

void MiauDelay::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    updateParameters();

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

   

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    delay.process(buffer, *apvts.getRawParameterValue("DelayTime"), totalNumInputChannels);
}

//==============================================================================
bool MiauDelay::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MiauDelay::createEditor()
{
    return new NewProjectAudioProcessorEditor (*this);
}

//==============================================================================
void MiauDelay::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MiauDelay::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MiauDelay();
}
