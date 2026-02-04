#pragma once

#include "juce_audio_basics/juce_audio_basics.h"

class CrossFader
{
public:
    void prepare(double sampleRate, int fadeTimeMs);
    void requestBus(int newBus);
    float getNextValue();

    int getCurrentBus() { return currentBus_; }
    int getTargetBus() { return targetBus_; }

private:
    int currentBus_ = 0;
    int targetBus_ = 0;
    int requestedBus_ = 0;

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> fader_;

    void updateBusesWhenPossible();
};
