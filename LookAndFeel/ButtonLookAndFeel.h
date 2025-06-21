/*
  ==============================================================================

    ButtonLookAndFeel.h
    Created: 21 Jun 2025 10:40:26am
    Author:  masal

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class ButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
	ButtonLookAndFeel()
	{

		buttonImage = juce::ImageCache::getFromMemory(BinaryData::huella_png, BinaryData::huella_pngSize);

		//setColour(juce::Button::textColourId, juce::Colours::darkgreen);

		//setColour(juce::Button::backgroundColour, juce::Colours::darkgrey);
	}


	virtual ~ButtonLookAndFeel() {}

	void drawButtonBackground(juce::Graphics& buttonImage, juce::Button& button, const juce::Colour& backgroundColour, bool, bool isButtonDown) override;

private:
	juce::Image buttonImage;
};
