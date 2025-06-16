/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class MiauDelayAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Slider::Listener
{
public:
    MiauDelayAudioProcessorEditor (MiauDelay&);
    ~MiauDelayAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void sliderValueChanged(juce::Slider*) override;

	// Función para preparar los sliders
    void prepareSlider(juce::Slider& slider);

private:
    MiauDelay& audioProcessor;

    juce::Slider delayTimeSlider; //inGainSlider, dryWetSlider, feedbackSlider, dryWetSlider, outGainSlider, hpfSlider, lpfSlider, lfoSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>  delayTimeAttach; //gainAttach, rateAttach, panAttach, dryWetAttach;


    juce::Image backgroundImage;
    juce::ImageComponent backgroundComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MiauDelayAudioProcessorEditor)
};
