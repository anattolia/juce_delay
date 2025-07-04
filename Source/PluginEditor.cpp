/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MiauDelayAudioProcessorEditor::MiauDelayAudioProcessorEditor(MiauDelay& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
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

    // sync active
    syncActive.setClickingTogglesState(true);
    syncActiveAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "SyncActive", syncActive);
    syncActive.onClick = [this]
        {
            updateSliderVisibility();
        };
    syncActive.setImages(false, true, true, huellaImg, 0.5, juce::Colour(230, 255, 255), huellaImg, 0.5, juce::Colour(230, 255, 255), huellaImg, 0.8, juce::Colour(251, 222, 89));
    addAndMakeVisible(syncActive);

    // Sync time slider
    prepareSlider(syncTimeSlider);
    syncTimeSlider.setRange(1, 4, 1);  // 4 discrete positions
    syncTimeSlider.setValue(3);  // Default to 1/4
    syncTimeSlider.textFromValueFunction = [](double value) 
        {
        switch (static_cast<int>(value)) {
        case 1: return juce::String("1/16");
        case 2: return juce::String("1/8");
        case 3: return juce::String("1/4");
        case 4: return juce::String("1/2");
        case 5: return juce::String("1");
        default: return juce::String("1/4");
        }
     };
    syncTimeAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "SyncTime", syncTimeSlider);

	// sync triplets active
    syncTripletsActive.setClickingTogglesState(true);
    syncTripletsActive.setImages(false, true, true, tresilloImg, 0.3, juce::Colour(230, 255, 255), tresilloImg, 0.3, juce::Colour(230, 255, 255), tresilloImg, 1.0, juce::Colour(251, 222, 89));
    syncTripletsAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "SyncTripletsActive", syncTripletsActive);
    addAndMakeVisible(syncTripletsActive);

    // lfo slider
    prepareSlider(lfoSlider);
    lfoAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "LFO", lfoSlider);

    // lfo active
    lfoActive.setClickingTogglesState(true);
    lfoActive.setImages(false, true, true, huellaImg, 0.3, juce::Colour(230, 255, 255), huellaImg, 0.3, juce::Colour(230, 255, 255), huellaImg, 1.0, juce::Colour(251, 222, 89));
    lfoActiveAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "ActiveLFO", lfoActive);
    addAndMakeVisible(lfoActive);

    /* lfoActive.onClick = []
        {
            //  DBG("Click");
        };
   */

    // LFO choice combo
   // lfoChoiceCombo.addItem("Sine", 1);
   // lfoChoiceCombo.addItem("Triangle", 2);
   // lfoChoiceCombo.addItem("Saw", 3);
   // lfoChoiceCombo.addItem("Square", 4);
   // lfoChoiceCombo.setJustificationType(juce::Justification::centred);
   // addAndMakeVisible(lfoChoiceCombo);
   // lfoChoiceAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "LFOChoice", lfoChoiceCombo);

    // Set initial slider visibility based on sync state
    updateSliderVisibility();

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(700, 500);
}

MiauDelayAudioProcessorEditor::~MiauDelayAudioProcessorEditor()
{
    delayTimeSlider.setLookAndFeel(nullptr);
    delayTimeSlider.removeListener(this);

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

    lfoSlider.setLookAndFeel(nullptr);
    lfoSlider.removeListener(this);

    syncTimeSlider.setLookAndFeel(nullptr);
    syncTimeSlider.removeListener(this);
}

void MiauDelayAudioProcessorEditor::updateSliderVisibility()
{
    bool syncIsActive = syncActive.getToggleState();

    delayTimeSlider.setVisible(!syncIsActive);
    syncTimeSlider.setVisible(syncIsActive);
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
void MiauDelayAudioProcessorEditor::paint(juce::Graphics& g)
{
}

void MiauDelayAudioProcessorEditor::resized()
{
    backgroundComponent.setBounds(getLocalBounds());

    // Poner los sliders del delaytime en ms y en figuras en la misma posición 
    delayTimeSlider.setBounds(78, 165, 173, 42);
    syncTimeSlider.setBounds(78, 165, 173, 42);

    tapTempoComp.setBounds(49, 235, 20, 20);
    feedbackSlider.setBounds(78, 333, 173, 42);
    dryWetSlider.setBounds(418, 346, 173, 42);

    syncActive.setBounds(123, 235, 20, 20);
    syncTripletsActive.setBounds(156, 240, 62, 31);

    inputGainSlider.setBounds(418, 55, 173, 42);
    outputGainSlider.setBounds(418, 423, 173, 42);

    hpfSlider.setBounds(418, 155, 173, 42);
    lpfSlider.setBounds(418, 228, 173, 42);

    lfoSlider.setBounds(78, 422, 173, 42);
    lfoActive.setBounds(196, 399, 20, 20);
}

void MiauDelayAudioProcessorEditor::sliderValueChanged(juce::Slider* s)
{
    if (s == &inputGainSlider)
    {
        // DBG ("feedbackSlider changed: " << feedbackSlider.getValue());
    }
}