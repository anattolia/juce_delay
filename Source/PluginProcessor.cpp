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
    // Crear los presets por defecto en la primera ejecución
    createDefaultPresets();
}

MiauDelay::~MiauDelay()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout MiauDelay::createParameters()
{
    juce::AudioProcessorValueTreeState::ParameterLayout parameters;
//    auto waves = juce::StringArray("Sine", "Square", "Saw", "Triangle");

    // Parámetros
    parameters.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{ "DelayTime", 1 }, "DelayTime", 0, 5000, 250));
    parameters.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ "DryWet", 1 }, "DryWet", 0.0f, 1.0f, 0.5f));
    parameters.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ "Feedback", 1 }, "Feedback", 0.0f, 0.99f, 0.5f));
    parameters.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ "InputGain", 1 }, "InputGain", 0.0f, 2.0f, 1.0f));
    parameters.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ "OutputGain", 1 }, "OutputGain", 0.0f, 2.0f, 1.0f));
    parameters.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{ "HPFFreq", 1 }, "HPFFreq", 0, 500, 0));
    parameters.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{ "LPFFreq", 1 }, "LPFFreq", 2000, 20000, 20000));
    parameters.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{ "SyncActive", 1 }, "SyncActive", false));
    parameters.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{ "SyncTime", 1 }, "SyncTime", juce::StringArray{ "1/16", "1/8", "1/4", "1/2", "1"}, 0));
    parameters.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{ "SyncTripletsActive", 1 }, "SyncTripletsActive", false));
    parameters.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ "LFO", 1 }, "LFO", 0.01f, 500.0f, 200.0f));
    parameters.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{ "ActiveLFO", 1 }, "ActiveLFO", false));

    // parámetro para cambiar el tipo de onda del LFO, por ahora no se usa
    // parameters.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{ "LFOChoice", 1 }, "LFOChoice", waves, 0));

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
    bool inSyncActive = *apvts.getRawParameterValue("SyncActive");
    int inSyncTimeValue = *apvts.getRawParameterValue("SyncTime");
    int inSyncTripletsActive = *apvts.getRawParameterValue("SyncTripletsActive");
   
    const float pi = juce::MathConstants<float>::pi;

    if (inSyncActive)	
    {
        if (getPlayHead() != nullptr)  
        {
            auto bpmValue = getPlayHead()->getPosition()->getBpm();
          
            if (bpmValue.hasValue())
            {
                // Convert sync time value to milliseconds
                inDelayTimeValue = syncTimeHandler.getSyncTimeInterval(inSyncTimeValue, bpmValue);
            }
        }
    }
	
   delay.setDelayTimeValue(inDelayTimeValue);
   delay.setFeedbackValue(inFeedbackValue);
   dryWet.setDryWetValue(inDryWetValue);
   inputGain.setGainValue(inInputGainValue);
   outputGain.setGainValue(inOutputGainValue);
   syncTimeHandler.setSyncActive(inSyncActive);
   syncTimeHandler.setTripletsActive(inSyncTripletsActive);
   hpf.setHPFFreqValue(inHPFFreqValue);
   lpf.setLPFFreqValue(inLPFFreqValue);
   lfo.setFrequencyValue(inLFOFreqValue);
   lfoActive = inLFOActive;
 //  lfo.setChoiceValue(*apvts.getRawParameterValue("LFOChoice"));
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

    inputGain.process(buffer);

    delay.process(buffer, totalNumInputChannels);

    if (lfoActive)
    {
        lfo.process(buffer);
    }
  
    // TODO: unificar en una sola funci�n?
    hpf.processHPF(buffer);
    lpf.processLPF(buffer);
   
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
// Preset Management
//==============================================================================

juce::File MiauDelay::getPresetDirectory()
{
    auto userDocuments = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    auto presetDir = userDocuments.getChildFile("MiauDelay").getChildFile("Presets");

    if (!presetDir.exists())
        presetDir.createDirectory();

    return presetDir;
}

void MiauDelay::savePreset(const juce::String& presetName)
{
    auto presetDir = getPresetDirectory();
    auto presetFile = presetDir.getChildFile(presetName + ".xml");

    // Get current state as XML
    juce::MemoryBlock memoryBlock;
    getStateInformation(memoryBlock);

    auto xmlState = getXmlFromBinary(memoryBlock.getData(), static_cast<int>(memoryBlock.getSize()));

    if (xmlState != nullptr)
    {
        xmlState->writeTo(presetFile);
        currentPresetName = presetName;
    }
}

void MiauDelay::loadPreset(const juce::String& presetName)
{
    auto presetDir = getPresetDirectory();
    auto presetFile = presetDir.getChildFile(presetName + ".xml");

    if (presetFile.existsAsFile())
    {
        auto xmlState = juce::XmlDocument::parse(presetFile);

        if (xmlState != nullptr)
        {
            juce::MemoryBlock memoryBlock;
            copyXmlToBinary(*xmlState, memoryBlock);
            setStateInformation(memoryBlock.getData(), (int)memoryBlock.getSize());
            currentPresetName = presetName;
        }
    }
}

juce::StringArray MiauDelay::getPresetList()
{
    juce::StringArray presetList;
    auto presetDir = getPresetDirectory();

    juce::RangedDirectoryIterator iter(presetDir, false, "*.xml");

    for (auto& entry : iter)
    {
        auto fileName = entry.getFile().getFileNameWithoutExtension();
        presetList.add(fileName);
    }

    presetList.sort(false); // Sort alphabetically
    return presetList;
}

void MiauDelay::createDefaultPresets()
{
    auto presetDir = getPresetDirectory();

    // Solo crea el default si no existe ningún preset
    if (getPresetList().isEmpty())
    {
        // No están funcionando los valores booleanos
        // Delay corto (gatito apurado)
        apvts.getParameter("DelayTime")->setValueNotifyingHost(0.05f);
        apvts.getParameter("Feedback")->setValueNotifyingHost(0.3f);
        apvts.getParameter("DryWet")->setValueNotifyingHost(0.5f);
        apvts.getParameter("SyncActive")->setValueNotifyingHost(0.0f);
        apvts.getParameter("SyncTripletsActive")->setValueNotifyingHost(0.0f);
        apvts.getParameter("ActiveLFO")->setValueNotifyingHost(0.0f);
        savePreset("Gatito Apurado");

        // Eco largo (gato despistado)
        apvts.getParameter("SyncActive")->setValueNotifyingHost(0.0f);
		apvts.getParameter("DelayTime")->setValueNotifyingHost(0.6f); // 3000 ms
        apvts.getParameter("Feedback")->setValueNotifyingHost(0.6);
        apvts.getParameter("DryWet")->setValueNotifyingHost(0.5f);
        savePreset("Gato Despistado");

		// Sync con 1/4 (michi sincronizado)
        apvts.getParameter("SyncActive")->setValueNotifyingHost(1.0f);
		apvts.getParameter("SyncTime")->setValueNotifyingHost(0.666f); // 1/4 note (index 2, normalized)
        apvts.getParameter("Feedback")->setValueNotifyingHost(0.4f);
		savePreset("Michi Sincronizado");

		// Delay con tresillo (aristogato)
        apvts.getParameter("SyncActive")->setValueNotifyingHost(1.0f);
		apvts.getParameter("SyncTripletsActive")->setValueNotifyingHost(1.0f);
        apvts.getParameter("SyncTime")->setValueNotifyingHost(0.666f); // 1/4 note (index 2, normalized)
        apvts.getParameter("Feedback")->setValueNotifyingHost(0.4f);
        savePreset("Aristogato");

        // Modulador activo (Gato cantor)
        apvts.getParameter("SyncActive")->setValueNotifyingHost(0.0f);
        apvts.getParameter("DelayTime")->setValueNotifyingHost(0.1f);  // 500ms
        apvts.getParameter("ActiveLFO")->setValueNotifyingHost(1.0f);
		apvts.getParameter("LFO")->setValueNotifyingHost(0.4f);
		apvts.getParameter("Feedback")->setValueNotifyingHost(0.5f);
		savePreset("Gato Cantor");
        
        // Modulador activo (Gato ronco)
        apvts.getParameter("SyncActive")->setValueNotifyingHost(0.0f);
        apvts.getParameter("DelayTime")->setValueNotifyingHost(0.05f);  // 500ms
        apvts.getParameter("ActiveLFO")->setValueNotifyingHost(1.0f);
        apvts.getParameter("LFO")->setValueNotifyingHost(0.03f);
        apvts.getParameter("Feedback")->setValueNotifyingHost(0.5f);
        apvts.getParameter("HPFFreq")->setValueNotifyingHost(0.0f);
        apvts.getParameter("LPFFreq")->setValueNotifyingHost(0.0f);
        savePreset("Gato Ronco");

        // Preset por defecto para guardar en la lista
        apvts.getParameter("DelayTime")->setValueNotifyingHost(0.05f);
        apvts.getParameter("Feedback")->setValueNotifyingHost(0.5f);
        apvts.getParameter("DryWet")->setValueNotifyingHost(0.5f);
        apvts.getParameter("InputGain")->setValueNotifyingHost(1.0f);
        apvts.getParameter("OutputGain")->setValueNotifyingHost(1.0f);
        apvts.getParameter("HPFFreq")->setValueNotifyingHost(0.0f);
        apvts.getParameter("LPFFreq")->setValueNotifyingHost(1.0f);
        apvts.getParameter("SyncActive")->setValueNotifyingHost(0.0f);
        apvts.getParameter("SyncTripletsActive")->setValueNotifyingHost(0.0f);
        apvts.getParameter("ActiveLFO")->setValueNotifyingHost(0.0f);
        savePreset("Default");
        
        
        // Preset por defecto
        apvts.getParameter("DelayTime")->setValueNotifyingHost(0.05f);
		apvts.getParameter("Feedback")->setValueNotifyingHost(0.5f);
		apvts.getParameter("DryWet")->setValueNotifyingHost(0.5f);
		apvts.getParameter("InputGain")->setValueNotifyingHost(1.0f);
		apvts.getParameter("OutputGain")->setValueNotifyingHost(1.0f);
		apvts.getParameter("HPFFreq")->setValueNotifyingHost(0.0f);
		apvts.getParameter("LPFFreq")->setValueNotifyingHost(1.0f);
        apvts.getParameter("SyncActive")->setValueNotifyingHost(0.0f);
        apvts.getParameter("SyncTripletsActive")->setValueNotifyingHost(0.0f);
        apvts.getParameter("ActiveLFO")->setValueNotifyingHost(0.0f);
		currentPresetName = "Default";  
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MiauDelay();
}
