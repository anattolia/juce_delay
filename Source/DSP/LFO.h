/*
  ==============================================================================

    LFO.h
    Created: 22 Jun 2025 12:25:42pm
    Author:  masal

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class LFO
{
public:
    LFO();
    ~LFO();

    enum Wave
    {
        Sine = 0,
        Square = 1,
        Saw = 2,
        Triangle = 3,
    };

    void prepare(double theSampleRate);
    void process(juce::AudioBuffer<float>& buffer);

    void setFrequencyValue(float newFrequency);
    void setChoiceValue(int newChoiceValue);

    float sineWave(int channel);
    float squareWave(int channel);
    float triangleWave(int channel);
    float sawtoothWave(int channel);

private:
    float sampleRate{ 44100.0f };
    float frequencyValue{ 20.0f };
    float twoPi = juce::MathConstants<float>::twoPi;
    float time[2];
    float deltaTime[2];
	int choiceValue { 0 };
};