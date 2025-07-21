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
		imageToggleOn = juce::ImageCache::getFromMemory(BinaryData::toggleon_png, BinaryData::toggleon_pngSize);
		imageToggleOff = juce::ImageCache::getFromMemory(BinaryData::toggleoff_png, BinaryData::toggleoff_pngSize);

	
		//setColour(juce::Button::backgroundColour, juce::Colours::darkgrey);
		
	}

	virtual ~ButtonLookAndFeel() {}

	void setImages(juce::Graphics& buttonImage, juce::Graphics& imageToggleOn, juce::Graphics& imageToggleOff) {
	}

	/*void drawButtonBackground(juce::Graphics& buttonImage, juce::Button& button, const juce::Colour& backgroundColour, bool, bool isButtonDown)
	{
		buttonImage.fillRect(100, 100, 100, 100);
		buttonImage.setColour(juce::Colours::aqua);
		//buttonImage.drawImage(buttonImage, )
	}*/

private:
	juce::Image buttonImage;
	juce::Image imageToggleOn;
	juce::Image imageToggleOff;



};
