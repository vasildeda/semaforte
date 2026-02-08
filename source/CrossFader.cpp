#include "CrossFader.h"

void CrossFader::prepare(double sampleRate, int fadeTimeMs)
{
    activeBus_ = requestedBus_ = 0;
    phase_ = Phase::Idle;

    fader_.reset(sampleRate, fadeTimeMs / 1000.0);
    fader_.setCurrentAndTargetValue(1.0);
}

void CrossFader::requestBus(int requestedBus)
{
    requestedBus_ = requestedBus;
}

CrossFader::State CrossFader::getNextState()
{
    if (phase_ == Phase::Idle && requestedBus_ != activeBus_)
    {
        phase_ = Phase::FadingOut;
        fader_.setCurrentAndTargetValue(1.0);
        fader_.setTargetValue(0.0);
    }

    float g = fader_.getNextValue();

    if (phase_ == Phase::FadingOut && !fader_.isSmoothing())
    {
        activeBus_ = requestedBus_;
        phase_ = Phase::FadingIn;
        fader_.setCurrentAndTargetValue(0.0);
        fader_.setTargetValue(1.0);
        g = fader_.getNextValue();
    }

    if (phase_ == Phase::FadingIn && !fader_.isSmoothing())
    {
        phase_ = Phase::Idle;
    }

    return { activeBus_, g };
}
