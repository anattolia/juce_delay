#include "TapTempoComponent.h"

TapTempoComponent::TapTempoComponent()
{
    tapButton.setImages(false, true, true, buttonImage, 0.01, juce::Colour(230, 255, 255), buttonImage, 0.3, juce::Colours::black, buttonImage, 0.3, juce::Colour(251, 222, 89));
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
    // Obtener el tempo actual en milisegundos
    double currentTime = juce::Time::getMillisecondCounterHiRes();

    if (lastTapTime > 0.0)
    {
        double interval = currentTime - lastTapTime;

        // Calcular BPM asumiendo un tap tempo de negra (quarter note)
        double bpm = 60000.0 / interval;

        // Convertir BPM en delay time en ms
        // Por un delay de negra, delay time = 60000 / BPM que es igual al intervalo
        double newDelayTime = interval;

        // Restringir el tiempo de delay (en este caso, entre 0 y 5000 ms).
        newDelayTime = juce::jlimit(0.0, 5000.0, newDelayTime);

        // Call the callback if one is set.
        if (tapCallback)
            tapCallback(newDelayTime);
    }

    lastTapTime = currentTime;
}