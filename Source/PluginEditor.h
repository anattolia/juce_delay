/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "../LookAndFeel/SliderLookAndFeel.h"
#include "GUI/TapTempoComponent.h"

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

    juce::Slider delayTimeSlider, dryWetSlider, inputGainSlider, outputGainSlider, hpfSlider, lpfSlider; // feedbackSlider, hpfSlider, lpfSlider, lfoSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>  delayTimeAttach, dryWetAttach, inputGainAttach, outputGainAttach, hpfAttach, lpfAttach; //gainAttach, rateAttach, panAttach;

    juce::Image backgroundImage;
    juce::ImageComponent backgroundComponent;

    SliderLookAndFeel sliderLookAndFeel;
    
    TapTempoComponent tapTempoComp;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MiauDelayAudioProcessorEditor)
};
