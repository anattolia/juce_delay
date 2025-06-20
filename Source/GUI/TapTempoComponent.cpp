#include "TapTempoComponent.h"

TapTempoComponent::TapTempoComponent()
{
    addAndMakeVisible(tapButton);
    tapButton.onClick = [this] { tapButtonClicked(); };
}

TapTempoComponent::~TapTempoComponent() {}

void TapTempoComponent::resized()
{
    tapButton.setBounds(getLocalBounds());
}

void TapTempoComponent::setTapCallback(std::function<void(double)> callback)
{
    tapCallback = callback;
}

void TapTempoComponent::reset()
{
    lastTapTime = 0.0;
}

void TapTempoComponent::tapButtonClicked()
{
    // Get current time in milliseconds using a high-res timer.
    double currentTime = juce::Time::getMillisecondCounterHiRes();
    
    if (lastTapTime > 0.0)
    {
        double interval = currentTime - lastTapTime;

        // Calculate BPM assuming a quarter note tap tempo.
        double bpm = 60000.0 / interval;

        // Convert BPM into a delay time in ms.
        // For a quarter note delay, delay time = 60000 / BPM which is just equal to interval.
        double newDelayTime = interval;
        
        // Clamp the delay time (in this example, between 0 and 5000 ms).
        newDelayTime = juce::jlimit(0.0, 5000.0, newDelayTime);
        
        // Call the callback if one is set.
        if (tapCallback)
            tapCallback(newDelayTime);
    }
    
    lastTapTime = currentTime;
}