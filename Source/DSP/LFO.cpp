/*
  ==============================================================================

    LFO.cpp
    Created: 22 Jun 2025 12:25:42pm
    Author:  masal

  ==============================================================================
*/

#include "LFO.h"

LFO::LFO() {};
LFO::~LFO() {};

void LFO::prepare(double theSampleRate)
{
	sampleRate = static_cast<float>(theSampleRate);
	
    for (int i = 0; i < 2; i++)
    {
        time[i] = 0.0f;
		deltaTime[i] = 1.0f / sampleRate;
    }
}

void LFO::process(juce::AudioBuffer<float>& buffer)
{
    for (int channel = 0; channel < buffer.getNumChannels(); channel++)
    {
        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
			float sample = buffer.getSample(channel, i);
			float waveValue = 0.0f;

            switch (choiceValue)
            {
            case Wave::Sine:
                waveValue = sineWave(channel);
                break;

            case Wave::Triangle:
				waveValue = triangleWave(channel);
				break;

            case Wave::Saw:
				waveValue = sawtoothWave(channel);

            case Wave::Square:
				waveValue = squareWave(channel);
                break;
            }

            float y = sample * waveValue;
            buffer.setSample(channel, i, y);

			time[channel] += deltaTime[channel];

            // Reset time variable
			if (time[channel] >= 1.0f)
			{
				time[channel] = 0.0f;
			}
        }
    }
}

void LFO::setFrequencyValue(float freqValue)
{
    frequencyValue = freqValue;
}

void LFO::setChoiceValue(int choice) 
{
    choiceValue = choice;
}

float LFO::sineWave(int channel)
{
    return 0.5f + (0.5f * sinf(twoPi * frequencyValue * time[channel]));
}

float LFO::squareWave(int channel)
{
    float fullPeriodTime = 1.0f / frequencyValue;
    float halfPeriodTime = fullPeriodTime / 2.0f;
    float localTime = fmod(time[channel], fullPeriodTime);

    if (localTime < halfPeriodTime)
    {
        return 1.0f;
    }
    else
    {
        return 0.0f;
    }
}

float LFO::triangleWave(int channel)
{
    float res = 0.0f;
    float fullPeriodTime = 1.0f / frequencyValue;
    float localTime = fmod(time[channel], fullPeriodTime);

    float value = localTime / fullPeriodTime;

    if (value < 0.25f)
    {
        res = value * 4.0f;
    }
    else if (value < 0.75f)
    {
        res = 2.0f - (value * 4.0f);
    }
    else
    {
        res = value * 4.0f - 4.0f;
    }
    return res;
}

float LFO::sawtoothWave(int channel)
{
    float fullPeriodTime = 1.0f / frequencyValue;
    float localTime = fmod(time[channel], fullPeriodTime);

    return (localTime / fullPeriodTime);

}