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

    // sync
    syncActive.setClickingTogglesState(true);
    syncActive.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::grey);
    syncActive.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::pink);
    syncActiveAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "SyncActive", syncActive);
    syncActive.onClick = [this]
        {
            updateSliderVisibility();
        };
    syncActive.setImages(false, true, true, huellaImage, 1.0, juce::Colours::grey, huellaImage, 0.3, juce::Colours::grey, huellaImage, 0.3, juce::Colours::greenyellow);
    addAndMakeVisible(syncActive);

    // Sync time slider
    prepareSlider(syncTimeSlider);
    syncTimeSlider.setRange(1, 4, 1);  // 4 discrete positions
    syncTimeSlider.setValue(3);  // Default to 1/4
    syncTimeSlider.textFromValueFunction = [](double value) {
        switch (static_cast<int>(value)) {
            case 1: return juce::String("1");
            case 2: return juce::String("1/2");
            case 3: return juce::String("1/4");
            case 4: return juce::String("1/8");
            default: return juce::String("1/4");
        }
    };
    syncTimeAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "SyncTime", syncTimeSlider);

    syncTripletsActive.setClickingTogglesState(true);
    syncTripletsActive.setImages(false, true, true, tresilloImage, 1.0, juce::Colours::grey, tresilloImage, 0.3, juce::Colours::grey, tresilloImage, 0.3, juce::Colours::greenyellow);

    syncTripletsAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "SyncTripletsActive", syncTripletsActive);
    addAndMakeVisible(syncTripletsActive);

    // lfo slider
    prepareSlider(lfoSlider);
    lfoAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "LFO", lfoSlider);
    lfoActive.setClickingTogglesState(true);

    addAndMakeVisible(lfoActive);
    lfoActive.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::white);
    lfoActive.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::darkgreen);

    lfoActiveAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "ActiveLFO", lfoActive);
    lfoActive.onClick = []
        {
          //  DBG("Click");
        };

	// LFO choice combo
   // lfoChoiceCombo.addItem("Sine", 1);
   // lfoChoiceCombo.addItem("Triangle", 2);
   // lfoChoiceCombo.addItem("Saw", 3);
   // lfoChoiceCombo.addItem("Square", 4);
   // lfoChoiceCombo.setJustificationType(juce::Justification::centred);
   // addAndMakeVisible(lfoChoiceCombo);

   // lfoChoiceAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "LFOChoice", lfoChoiceCombo);

    // Preset management UI
    presetCombo.setJustificationType(juce::Justification::centred);
    presetCombo.setTextWhenNothingSelected("Select Preset");
    presetCombo.onChange = [this] {
        if (presetCombo.getSelectedId() > 0) {
            auto selectedPreset = presetCombo.getText();
            audioProcessor.loadPreset(selectedPreset);
        }
    };
    addAndMakeVisible(presetCombo);
    
    savePresetButton.onClick = [this] { savePresetButtonClicked(); };
    addAndMakeVisible(savePresetButton);
    
    loadPresetButton.onClick = [this] { loadPresetButtonClicked(); };
    addAndMakeVisible(loadPresetButton);
    
    updatePresetCombo();

    // Set initial slider visibility based on sync state
    updateSliderVisibility();

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (700, 500);
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
void MiauDelayAudioProcessorEditor::paint (juce::Graphics& g)
{
}

void MiauDelayAudioProcessorEditor::resized()
{
    backgroundComponent.setBounds(getLocalBounds());

    // Position both delay time sliders in the same location
    delayTimeSlider.setBounds(78, 167, 173, 42);
    syncTimeSlider.setBounds(78, 167, 173, 42);
    
    tapTempoComp.setBounds(49, 235, 24, 24);
    feedbackSlider.setBounds(78, 341, 173, 42);
	dryWetSlider.setBounds(418, 346, 173, 42);

	syncActive.setBounds(119, 235, 24, 24);
    syncTripletsActive.setBounds(250, 245, 55, 24);

    inputGainSlider.setBounds(418, 55, 173, 42);
    outputGainSlider.setBounds(418, 428, 173, 42);

    hpfSlider.setBounds(418, 158, 173, 42);
    lpfSlider.setBounds(418, 231, 173, 42);

    lfoSlider.setBounds(78, 430, 173, 42);
    lfoActive.setBounds(201, 410, 16, 17);
    
    // Preset management UI positioning (top area)
    presetCombo.setBounds(50, 20, 150, 25);
    savePresetButton.setBounds(210, 20, 60, 25);
    loadPresetButton.setBounds(280, 20, 60, 25);
}

void MiauDelayAudioProcessorEditor::sliderValueChanged(juce::Slider* s)
{
    if (s == &inputGainSlider)
    {
      // DBG ("feedbackSlider changed: " << feedbackSlider.getValue());
    }
}

//==============================================================================
// Preset Management Methods
//==============================================================================

void MiauDelayAudioProcessorEditor::updatePresetCombo()
{
    presetCombo.clear();
    auto presetList = audioProcessor.getPresetList();
    
    for (int i = 0; i < presetList.size(); ++i)
    {
        presetCombo.addItem(presetList[i], i + 1);
    }
    
    // Update the selection to match current preset
    auto currentPreset = audioProcessor.getCurrentPresetName();
    if (currentPreset.isNotEmpty())
    {
        presetCombo.setText(currentPreset, juce::dontSendNotification);
    }
}

void MiauDelayAudioProcessorEditor::savePresetButtonClicked()
{
    auto* alertWindow = new juce::AlertWindow("Save Preset", "Enter a name for your preset:", juce::AlertWindow::QuestionIcon);
    alertWindow->addTextEditor("presetName", "New Preset", "Preset Name:");
    alertWindow->addButton("Save", 1, juce::KeyPress(juce::KeyPress::returnKey, 0, 0));
    alertWindow->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey, 0, 0));
    
    alertWindow->enterModalState(true, juce::ModalCallbackFunction::create([this, alertWindow](int result)
    {
        if (result == 1) // if user pressed save
        {
            auto presetName = alertWindow->getTextEditorContents("presetName");
            if (presetName.isNotEmpty())
            {
                audioProcessor.savePreset(presetName);
                updatePresetCombo();
                presetCombo.setText(presetName, juce::dontSendNotification);
            }
        }
        delete alertWindow;
    }));
}

void MiauDelayAudioProcessorEditor::loadPresetButtonClicked()
{
    updatePresetCombo(); // Refresh the list in case new presets were added externally
}