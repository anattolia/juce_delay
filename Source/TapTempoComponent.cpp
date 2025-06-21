/*
  ==============================================================================

    TapTempoComponent.cpp
    Created: 20 Jun 2025 11:07:39pm
    Author:  masal

  ==============================================================================
*/

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
    // Obtener el tiempo actual en milisegundos usando un timer de alta resolución
    double currentTime = juce::Time::getMillisecondCounterHiRes();

    if (lastTapTime > 0.0)
    {
        double interval = currentTime - lastTapTime;

        // Calcular el BPM asumiendo un tap tempo de negra
        double bpm = 60000.0 / interval;

        // Convertir el BPM a un tiempo de delay en milisegundos
        // Para un delay de negra, delay time = 60000 / BPM, que es igual al intervalo
        double newDelayTime = interval;

        // Restringir el tiempo de delay (en este ejemplo entre 0 y 5000 ms)
        newDelayTime = juce::jlimit(0.0, 5000.0, newDelayTime);

        // Llamar el callback si hay uno definido
        if (tapCallback)
            tapCallback(newDelayTime);
    }
    lastTapTime = currentTime;
}