/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MiauDelayAudioProcessorEditor::MiauDelayAudioProcessorEditor (MiauDelay& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Background
    backgroundImage = juce::ImageFileFormat::loadFrom(BinaryData::miau_png, BinaryData::miau_pngSize);
    backgroundComponent.setImage(backgroundImage);
    addAndMakeVisible(backgroundComponent);

    // Delay time slider
    prepareSlider(delayTimeSlider);
    delayTimeAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DelayTime", delayTimeSlider);

    // Feedback slider
    prepareSlider(feedbackSlider);
    feedbackAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "Feedback", feedbackSlider);

    // DryWet slider
    prepareSlider(dryWetSlider);
    dryWetAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DryWet", dryWetSlider);

    // inputGain slider
    prepareSlider(inputGainSlider);
    inputGainAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "InputGain", inputGainSlider);
    
    // outputGain slider
    prepareSlider(outputGainSlider);
    outputGainAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "OutputGain", outputGainSlider);

    // hpf slider
    prepareSlider(hpfSlider);
    hpfAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "HPFFreq", hpfSlider);

    // lpf slider
    prepareSlider(lpfSlider);
    lpfAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "LPFFreq", lpfSlider);

    // Configurar el componente de tap tempo
    // Callback para actualizar el parámetro DelayTime mapeando newDelayTime de ms a un valor normalizado
    tapTempoComp.setTapCallback([this](double newDelayTime)
        {
            float normalizedValue = static_cast<float>(newDelayTime / 5000.00f);
            audioProcessor.apvts.getParameter("DelayTime")->setValueNotifyingHost(normalizedValue);
           // DBG("Nuevo delay time desde TapTempo: " << newDelayTime << "ms");
        }
    );
    addAndMakeVisible(tapTempoComp);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (700, 500);
}

MiauDelayAudioProcessorEditor::~MiauDelayAudioProcessorEditor()
{
  delayTimeSlider.setLookAndFeel(nullptr);
  delayTimeSlider.removeListener(this);

 // tapTempoComp.setLookAndFeel(nullptr);
 // tapTempoComp.removeComponentListener(this);

  feedbackSlider.setLookAndFeel(nullptr);
  feedbackSlider.removeListener(this);

  dryWetSlider.setLookAndFeel(nullptr);
  dryWetSlider.removeListener(this);

  inputGainSlider.setLookAndFeel(nullptr);
  inputGainSlider.removeListener(this);

  outputGainSlider.setLookAndFeel(nullptr);
  outputGainSlider.removeListener(this);

  hpfSlider.setLookAndFeel(nullptr);
  hpfSlider.removeListener(this);

  lpfSlider.setLookAndFeel(nullptr);
  lpfSlider.removeListener(this);
}

void MiauDelayAudioProcessorEditor::prepareSlider(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 55, 20);
    addAndMakeVisible(slider);

   slider.setLookAndFeel(&sliderLookAndFeel);
   slider.addListener(this);
}

//==============================================================================
void MiauDelayAudioProcessorEditor::paint (juce::Graphics& g)
{
}

void MiauDelayAudioProcessorEditor::resized()
{
    backgroundComponent.setBounds(getLocalBounds());

    delayTimeSlider.setBounds(78, 167, 173, 42);
    tapTempoComp.setBounds(32, 230, 50, 65);
    feedbackSlider.setBounds(78, 341, 173, 42);
	dryWetSlider.setBounds(418, 346, 173, 42);
    inputGainSlider.setBounds(418, 55, 173, 42);
    outputGainSlider.setBounds(418, 428, 173, 42);
    hpfSlider.setBounds(418, 158, 173, 42);
    lpfSlider.setBounds(418, 231, 173, 42);
}

void MiauDelayAudioProcessorEditor::sliderValueChanged(juce::Slider* s)
{
    if (s == &inputGainSlider)
    {
       DBG ("feedbackSlider changed: " << feedbackSlider.getValue());
    }
}