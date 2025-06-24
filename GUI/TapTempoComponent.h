#pragma once

#include <JuceHeader.h>
#include <functional>

class TapTempoComponent : public juce::Component
{
public:
    TapTempoComponent();
    ~TapTempoComponent() override;

    void resized() override;

    /** Configurar un callback que se llama cada vez que un nuevo intervalo de tap es computado.
        El callback recibe el tiempo de delay computado en milisegundos. */
    void setTapCallback(std::function<void(double)> callback);

    /** Opcional, borrar el historial del tap (útil si pasa mucho tiempo entre taps). */
    void reset();

private:
    void tapButtonClicked();

    juce::Image buttonImage{ juce::ImageCache::getFromMemory(BinaryData::huella_png, BinaryData::huella_pngSize) };

    juce::ImageButton tapButton;
   
    double lastTapTime = 0.0;
    std::function<void(double)> tapCallback;
};