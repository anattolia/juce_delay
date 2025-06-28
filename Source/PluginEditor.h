/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "../LookAndFeel/SliderLookAndFeel.h"
#include "../LookAndFeel/ButtonLookAndFeel.h"
#include "../GUI/TapTempoComponent.h"

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

  //  void prepareButton(juce::Button& button);

private:
    MiauDelay& audioProcessor;

    juce::Slider delayTimeSlider, feedbackSlider, dryWetSlider, inputGainSlider, outputGainSlider, hpfSlider, lpfSlider, lfoSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>  delayTimeAttach, feedbackAttach, dryWetAttach, inputGainAttach, outputGainAttach, hpfAttach, lpfAttach, lfoAttach; //, syncActiveAttach;

    juce::ImageButton syncActive;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> syncActiveAttach;

    juce::ComboBox syncChoiceCombo;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> syncChoiceAttach;

    juce::ImageButton syncTripletsActive;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> syncTripletsAttach;

    juce::TextButton lfoActive;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> lfoActiveAttach;

    juce::ComboBox lfoChoiceCombo;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> lfoChoiceAttach;

    juce::Image backgroundImage;
    juce::ImageComponent backgroundComponent;

    juce::Image huellaImage{ juce::ImageCache::getFromMemory(BinaryData::huella_png, BinaryData::huella_pngSize) };
    juce::Image tresilloImage{ juce::ImageCache::getFromMemory(BinaryData::tresillo_png, BinaryData::tresillo_pngSize) };


    SliderLookAndFeel sliderLookAndFeel;
    //ButtonLookAndFeel buttonLookAndFeel;

    TapTempoComponent tapTempoComp;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MiauDelayAudioProcessorEditor)
};
