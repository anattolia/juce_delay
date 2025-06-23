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
    auto waves = juce::StringArray("Sine", "Square", "Saw", "Triangle");

    // Par�metros del tiempo del delay
    parameters.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{ "DelayTime", 1 }, "DelayTime", 0, 5000, 250));
    parameters.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ "DryWet", 1 }, "DryWet", 0.0f, 1.0f, 0.5f));
    parameters.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ "Feedback", 1 }, "Feedback", 0.0f, 0.99f, 0.5f));
    parameters.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ "InputGain", 1 }, "InputGain", 0.0f, 2.0f, 1.0f));
    parameters.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ "OutputGain", 1 }, "OutputGain", 0.0f, 2.0f, 1.0f));
    parameters.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{ "HPFFreq", 1 }, "HPFFreq", 0, 500, 0));
    parameters.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{ "LPFFreq", 1 }, "LPFFreq", 2000, 20000, 20000));
    parameters.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{ "SyncMode", 1 }, "SyncMode", false));
    parameters.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{ "SyncTime", 1 }, "SyncTime", juce::StringArray{ "1", "1/2", "1/4", "1/8"}, 0));
    parameters.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ "LFO", 1 }, "LFO", 0.01f, 20.0f, 10.0f));
    parameters.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{ "LFOChoice", 1 }, "LFOChoice", waves, 0));
    parameters.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{ "ActiveLFO", 1 }, "ActiveLFO", false));

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
    juce::dsp::ProcessSpec spec;

    // �Esto hace falta?
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();
    spec.maximumBlockSize = samplesPerBlock;

	delay.prepare(sampleRate, samplesPerBlock);
    hpf.prepare(spec);
    lpf.prepare(spec);
    lfo.prepare(sampleRate);
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
    float inDelayTimeValue = *apvts.getRawParameterValue("DelayTime");
    float inFeedbackValue = *apvts.getRawParameterValue("Feedback");
    float inDryWetValue = *apvts.getRawParameterValue("DryWet");
    float inInputGainValue = *apvts.getRawParameterValue("InputGain");
    float inOutputGainValue = *apvts.getRawParameterValue("OutputGain");
    float inHPFFreqValue = *apvts.getRawParameterValue("HPFFreq");
    float inLPFFreqValue = *apvts.getRawParameterValue("LPFFreq");
    float inLFOFreqValue = *apvts.getRawParameterValue("LFO");
    bool inLFOActive = *apvts.getRawParameterValue("ActiveLFO");
   // auto inLFOChoiceValue = *apvts.getRawParameterValue("LFOChoice");
    bool inSyncMode = *apvts.getRawParameterValue("SyncMode");
    int inSyncTimeValue = *apvts.getRawParameterValue("SyncTime");
    

    if (inSyncMode)
    {
        if (getPlayHead() != nullptr)  
        {
            auto bpmValue = getPlayHead()->getPosition()->getBpm();

            if (bpmValue.hasValue())
            {
                // Convert sync time value to milliseconds
              //  syncTimeHandler.setSyncMode(inSyncTimeValue);
                inDelayTimeValue = syncTimeHandler.getSyncTimeInterval(inSyncMode, bpmValue);
            }
        }
    }
	
   delay.setDelayTimeValue(inDelayTimeValue);
   delay.setFeedbackValue(inFeedbackValue);
   dryWet.setDryWetValue(inDryWetValue);
   inputGain.setGainValue(inInputGainValue);
   outputGain.setGainValue(inOutputGainValue);
   hpf.setHPFFreqValue(inHPFFreqValue);
   lpf.setLPFFreqValue(inLPFFreqValue);
   lfo.setFrequencyValue(inLFOFreqValue);
   lfoActive = inLFOActive;
   lfo.setChoiceValue(*apvts.getRawParameterValue("LFOChoice")); // TODO ¿Por qué no se puede hacer como los demás?
}

void MiauDelay::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    updateParameters();

    dryBuffer.makeCopyOf(buffer);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    delay.process(buffer, totalNumInputChannels);
    inputGain.process(buffer);
  
    // TODO: unificar en una sola funci�n?
    hpf.processHPF(buffer);
    lpf.processLPF(buffer);

    if (lfoActive)
    {
        lfo.process(buffer);
    }

    dryWet.process(dryBuffer, buffer);
    outputGain.process(buffer);
}

//==============================================================================
bool MiauDelay::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MiauDelay::createEditor()
{
    return new MiauDelayAudioProcessorEditor (*this);
}

//==============================================================================
void MiauDelay::getStateInformation (juce::MemoryBlock& destData)
{
    std::unique_ptr<juce::XmlElement> xml(apvts.state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MiauDelay::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName(apvts.state.getType()))
        {
            auto state = juce::ValueTree::fromXml(*xmlState);
            apvts.replaceState(state);
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MiauDelay();
}
