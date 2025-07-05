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

    // Cambiar el slider que se muestra según el estado del sync
    void updateSliderVisibility();

private:
    MiauDelay& audioProcessor;

    juce::Slider delayTimeSlider, feedbackSlider, dryWetSlider, inputGainSlider, outputGainSlider, hpfSlider, lpfSlider, lfoSlider, syncTimeSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>  delayTimeAttach, feedbackAttach, dryWetAttach, inputGainAttach, outputGainAttach, hpfAttach, lpfAttach, lfoAttach, syncTimeAttach;

    juce::ImageButton syncActive;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> syncActiveAttach;

    juce::ImageButton syncTripletsActive;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> syncTripletsAttach;

    juce::ImageButton lfoActive;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> lfoActiveAttach;

    juce::ComboBox lfoChoiceCombo;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> lfoChoiceAttach;

    // Interfaz Preset management
    juce::ComboBox presetCombo;
    juce::TextButton savePresetButton{ "Save" };
    juce::TextButton loadPresetButton{ "Load" };

    void updatePresetCombo();
    void savePresetButtonClicked();
    void loadPresetButtonClicked();

    juce::Image backgroundImage;
    juce::ImageComponent backgroundComponent;

    juce::Image huellaImg{ juce::ImageCache::getFromMemory(BinaryData::huella_png, BinaryData::huella_pngSize) };
    juce::Image tresilloImg{ juce::ImageCache::getFromMemory(BinaryData::tresillo_png, BinaryData::tresillo_pngSize) };


    SliderLookAndFeel sliderLookAndFeel;

    TapTempoComponent tapTempoComp;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MiauDelayAudioProcessorEditor)
};
