#pragma once

#include <JuceHeader.h>
#include <functional>

class TapTempoComponent : public juce::Component
{
public:
    TapTempoComponent();
    ~TapTempoComponent() override;

    void resized() override;

    /** Set a callback that is invoked each time a new tap interval is computed.
        The callback receives the computed delay time in milliseconds. */
    void setTapCallback(std::function<void(double)> callback);

    /** Optionally, reset the tap history (useful if too much time elapses between taps). */
    void reset();

private:
    void tapButtonClicked();

    juce::TextButton tapButton{ "Tap" };
    double lastTapTime = 0.0;
    std::function<void(double)> tapCallback;
};