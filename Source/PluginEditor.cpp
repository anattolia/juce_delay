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


    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (700, 500);
}

MiauDelayAudioProcessorEditor::~MiauDelayAudioProcessorEditor()
{
  //  delayTimeSlider.setLookAndFeel(nullptr);
  //  delayTimeSlider.removeListener(this);
}

void MiauDelayAudioProcessorEditor::prepareSlider(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(slider);

   //slider.setLookAndFeel(&knobLookAndFeel);
   slider.addListener(this);
}

//==============================================================================
void MiauDelayAudioProcessorEditor::paint (juce::Graphics& g)
{
}

void MiauDelayAudioProcessorEditor::resized()
{
    backgroundComponent.setBounds(getLocalBounds());

    delayTimeSlider.setBounds(100, 300, 100, 100);
}

void MiauDelayAudioProcessorEditor::sliderValueChanged(juce::Slider* s)
{
    if (s == &delayTimeSlider)
    {
        //DBG("slider changed");
       DBG ("Slider changed: " << delayTimeSlider.getValue());
    }
}