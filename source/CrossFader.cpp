#include "CrossFader.h"

void CrossFader::prepare(double sampleRate, int fadeTimeMs)
{
    currentBus_ = targetBus_ = requestedBus_ = 0;

    fader_.reset(sampleRate, fadeTimeMs / 1000.0);
    fader_.setCurrentAndTargetValue(0.0);
}

void CrossFader::requestBus(int newBus)
{
    requestedBus_ = newBus;
}

float CrossFader::getNextValue()
{
    return fader_.getNextValue();
}

void CrossFader::updateBusesWhenPossible()
{
    if (!fader_.isSmoothing()) // i.e. idle
    {
        if (targetBus_ != requestedBus_)
        {
            currentBus_ = targetBus_;
            fader_.setCurrentAndTargetValue(0.0);
            targetBus_ = requestedBus_;
        }

        if (currentBus_ != targetBus_)
        {
            fader_.setTargetValue(1.0);
        }
    }
}
