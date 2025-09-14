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
    delayTimeSlider.setTextValueSuffix(" ms");

    // Feedback slider
    prepareSlider(feedbackSlider);
    feedbackAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "Feedback", feedbackSlider);

    // DryWet slider
    prepareSlider(dryWetSlider);
    dryWetAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DryWet", dryWetSlider);
    // convertir etiqueta de 0-1 a porcentaje
    dryWetSlider.textFromValueFunction = [](double value)
        {
            if (value <= 0.0)
                return juce::String("0 %");
            double percentage = value*100;
            return juce::String(percentage) + " %";
        };
    dryWetSlider.updateText();

    // inputGain slider
    prepareSlider(inputGainSlider);
    inputGainAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "InputGain", inputGainSlider);
    // convertir etiqueta de 0-1 a dBs
    inputGainSlider.textFromValueFunction = [](double value)
        {
            if (value <= 0.0)
                return juce::String("-inf dB");
            double dB = 20.0 * std::log10(value);
            return juce::String(dB, 1) + " dB";
        };
    inputGainSlider.updateText();
 
    // outputGain slider
    prepareSlider(outputGainSlider);
    outputGainAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "OutputGain", outputGainSlider);
    // convertir etiqueta de 0-1 a dBs
    outputGainSlider.textFromValueFunction = [](double value)
        {
            if (value <= 0.0)
                return juce::String("-inf dB");
            double dB = 20.0 * std::log10(value);
            return juce::String(dB, 1) + " dB";
        };
    outputGainSlider.updateText();

    // hpf slider
    prepareSlider(hpfSlider);
    hpfAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "HPFFreq", hpfSlider);
    hpfSlider.setTextValueSuffix(" Hz");

    // lpf slider
    prepareSlider(lpfSlider);
    lpfAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "LPFFreq", lpfSlider);
    lpfSlider.setTextValueSuffix(" Hz");

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
    syncActive.setImages(false, true, true, toggleOffImg, 0.9, juce::Colour(), toggleOffImg, 0.9, juce::Colour(), toggleOnImg, 0.9, juce::Colour());
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
    lfoSlider.setTextValueSuffix(" Hz");

    // lfo active
    lfoActive.setClickingTogglesState(true);
    lfoActive.setImages(false, true, true, toggleOffImg, 0.9, juce::Colour(), toggleOffImg, 0.9, juce::Colour(), toggleOnImg, 0.9, juce::Colour());
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

    // Interfaz de Preset managment
    presetCombo.setJustificationType(juce::Justification::centred);
    presetCombo.setTextWhenNothingSelected("Select Preset");
    presetCombo.setColour(juce::ComboBox::backgroundColourId, juce::Colour(158, 188, 194));
    presetCombo.onChange = [this] {
        if (presetCombo.getSelectedId() > 0) {
            auto selectedPreset = presetCombo.getText();
            audioProcessor.loadPreset(selectedPreset);
        }
        };
    addAndMakeVisible(presetCombo);

    savePresetButton.onClick = [this] { savePresetButtonClicked(); };
	savePresetButton.setColour(juce::TextButton::buttonColourId, juce::Colour(158, 188, 194));
    addAndMakeVisible(savePresetButton);

    // loadPresetButton.onClick = [this] { loadPresetButtonClicked(); };
    // addAndMakeVisible(loadPresetButton);

    updatePresetCombo();

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
   
    slider.setLookAndFeel(&sliderLookAndFeel);
    slider.addListener(this); 
    addAndMakeVisible(slider);
}

//==============================================================================
void MiauDelayAudioProcessorEditor::paint(juce::Graphics& g)
{
}

void MiauDelayAudioProcessorEditor::resized()
{
    backgroundComponent.setBounds(getLocalBounds());

    // Poner los sliders del delaytime en ms y en figuras en la misma posición 
    delayTimeSlider.setBounds(78, 160, 173, 42);
    syncTimeSlider.setBounds(78, 160, 173, 42);

    tapTempoComp.setBounds(50, 234, 20, 20);
    feedbackSlider.setBounds(78, 339, 173, 42);
    dryWetSlider.setBounds(418, 358, 173, 42);

    syncActive.setBounds(115, 237, 33, 15);
    syncTripletsActive.setBounds(154, 238, 62, 31);

    inputGainSlider.setBounds(418, 59, 173, 42);
    outputGainSlider.setBounds(418, 426, 173, 42);

    hpfSlider.setBounds(418, 158, 173, 42);
    lpfSlider.setBounds(418, 230, 173, 42);

    lfoSlider.setBounds(78, 426, 173, 42);
    lfoActive.setBounds(205, 410, 33, 15);

    // Ubicación del preset management
    presetCombo.setBounds(467, 6, 140, 16);
    savePresetButton.setBounds(616, 6, 50, 18);
    
    // presetCombo.setBounds(410, 9, 140, 20);
    // savePresetButton.setBounds(558, 9, 50, 20);
	// loadPresetButton.setBounds(614, 9, 50, 20);
}

void MiauDelayAudioProcessorEditor::sliderValueChanged(juce::Slider* s)
{
    if (s == &inputGainSlider)
    {
        // DBG ("feedbackSlider changed: " << feedbackSlider.getValue());
    }
}

//==============================================================================
// Métodos del Preset Management
//==============================================================================

void MiauDelayAudioProcessorEditor::updatePresetCombo()
{
    presetCombo.clear();
    auto presetList = audioProcessor.getPresetList();

    for (int i = 0; i < presetList.size(); ++i)
    {
        presetCombo.addItem(presetList[i], i + 1);
    }

    // ACtualizar la selección para que coincida con el preset actual
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
            if (result == 1) // si el usuario presiona save
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