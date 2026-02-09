#include "CrossFader.h"

void CrossFader::prepare(double sampleRate, int fadeTimeMs)
{
    fader_.reset(sampleRate, fadeTimeMs / 1000.0);
    fader_.setCurrentAndTargetValue(1.0f);
}

void CrossFader::mute()
{
    fader_.setTargetValue(0.0f);
}

void CrossFader::unmute()
{
    fader_.setTargetValue(1.0f);
}

float CrossFader::getNextGain()
{
    return fader_.getNextValue();
}
