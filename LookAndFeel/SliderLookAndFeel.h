/*
  ==============================================================================

	SliderLookAndFeel.h
	Created: 16 Jun 2025 3:52:39pm
	Author:  masal

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class SliderLookAndFeel : public juce::LookAndFeel_V4
{
public:

	SliderLookAndFeel()
	{
		setColour(juce::Slider::thumbColourId, juce::Colours::darkgreen);
		setColour(juce::Slider::trackColourId, juce::Colours::white);
		setColour(juce::Slider::backgroundColourId, juce::Colours::darkgrey);
	}

	virtual ~SliderLookAndFeel() {}

	void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
		float sliderPos, float minSliderPos, float maxSliderPos,
		const juce::Slider::SliderStyle style, juce::Slider& slider) override
	{
		// Draw the track
		g.setColour(findColour(juce::Slider::trackColourId));
		g.fillRect(x, y + height / 2 - 2, width, 4);
		// Draw the thumb
		g.setColour(findColour(juce::Slider::thumbColourId));
		g.fillRect(static_cast<int>(sliderPos - 5), y + height - 18, 15, 15);
	}

private:

};