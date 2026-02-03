#include "LongPressButton.h"

LongPressButton::LongPressButton(const juce::String& name,
                                 const void* normalSVGData, int normalSVGSize,
                                 const void* pressedSVGData, int pressedSVGSize,
                                 double longPressThresholdMs,
                                 LongPressMode mode)
    : juce::DrawableButton(name, juce::DrawableButton::ImageFitted),
      button(name, juce::DrawableButton::ImageFitted),
      longPressThreshold(longPressThresholdMs),
      longPressMode(mode)
{
    addAndMakeVisible(button);

    // Create drawables from SVG
    std::unique_ptr<juce::Drawable> normalDrawable = juce::Drawable::createFromImageData(normalSVGData, normalSVGSize);
    std::unique_ptr<juce::Drawable> pressedDrawable = nullptr;

    if (pressedSVGData && pressedSVGSize > 0)
        pressedDrawable = juce::Drawable::createFromImageData(pressedSVGData, pressedSVGSize);

    // Assign to button
    button.setImages(normalDrawable.get());
}

void LongPressButton::mouseDown(const juce::MouseEvent& e)
{
    mouseDownTime = juce::Time::getMillisecondCounterHiRes();
    isMouseDown = true;
    startTimer(10);
    juce::DrawableButton::mouseDown(e); // preserve default behavior
}

void LongPressButton::mouseUp(const juce::MouseEvent& e)
{
    stopTimer();
    auto duration = juce::Time::getMillisecondCounterHiRes() - mouseDownTime;

    if (duration >= longPressThreshold)
    {
        if (onLongPress) onLongPress();
    }
    else
    {
        if (onClick) onClick();
    }

    isMouseDown = false;
    juce::DrawableButton::mouseUp(e); // preserve default behavior
}

void LongPressButton::resized()
{
    button.setBounds(getLocalBounds());
}

void LongPressButton::timerCallback()
{
    if (isMouseDown && longPressMode == LongPressMode::TriggerDuringHold)
    {
        auto duration = juce::Time::getMillisecondCounterHiRes() - mouseDownTime;
        if (duration >= longPressThreshold)
        {
            stopTimer();
            isMouseDown = false;
            if (onLongPress) onLongPress();
        }
    }
}
