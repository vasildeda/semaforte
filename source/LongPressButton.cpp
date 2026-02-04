#include "LongPressButton.h"

LongPressButton::LongPressButton(const juce::String& name,
                                 const void* normalSVGData, int normalSVGSize,
                                 const void* pressedSVGData, int pressedSVGSize,
                                 double longPressThresholdMs,
                                 LongPressMode mode)
    : juce::DrawableButton(name, juce::DrawableButton::ImageFitted),
      button_(name, juce::DrawableButton::ImageFitted),
      longPressThreshold_(longPressThresholdMs),
      longPressMode_(mode)
{
    addAndMakeVisible(button_);

    // Create drawables from SVG
    std::unique_ptr<juce::Drawable> normalDrawable = juce::Drawable::createFromImageData(normalSVGData, normalSVGSize);
    std::unique_ptr<juce::Drawable> pressedDrawable = nullptr;

    if (pressedSVGData && pressedSVGSize > 0)
        pressedDrawable = juce::Drawable::createFromImageData(pressedSVGData, pressedSVGSize);

    // Assign to button
    button_.setImages(normalDrawable.get());
}

void LongPressButton::mouseDown(const juce::MouseEvent& e)
{
    mouseDownTime_ = juce::Time::getMillisecondCounterHiRes();
    isMouseDown_ = true;
    startTimer(10);
    juce::DrawableButton::mouseDown(e); // preserve default behavior
}

void LongPressButton::mouseUp(const juce::MouseEvent& e)
{
    stopTimer();
    auto duration = juce::Time::getMillisecondCounterHiRes() - mouseDownTime_;

    if (duration >= longPressThreshold_)
    {
        if (onLongPress) onLongPress();
    }
    else
    {
        if (onClick) onClick();
    }

    isMouseDown_ = false;
    juce::DrawableButton::mouseUp(e); // preserve default behavior
}

void LongPressButton::resized()
{
    button_.setBounds(getLocalBounds());
}

void LongPressButton::timerCallback()
{
    if (isMouseDown_ && longPressMode_ == LongPressMode::TriggerDuringHold)
    {
        auto duration = juce::Time::getMillisecondCounterHiRes() - mouseDownTime_;
        if (duration >= longPressThreshold_)
        {
            stopTimer();
            isMouseDown_ = false;
            if (onLongPress) onLongPress();
        }
    }
}
