#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>

/**
 * LongPressButton
 * A button that detects short clicks and long presses.
 * Displays text with colored outline based on state.
 */
class LongPressButton : public juce::Component,
                        private juce::Timer
{
public:
    LongPressButton();
    ~LongPressButton() override = default;

    void setText(const juce::String& text);
    void setSelected(bool selected);
    void setLearning(bool learning);

    bool isSelected() const { return selected_; }
    bool isLearning() const { return learning_; }

    // Lambda callbacks
    std::function<void()> onClick;      // short click
    std::function<void()> onLongPress;  // long press (200ms)

    void paint(juce::Graphics& g) override;

protected:
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;

private:
    juce::String text_ { "--" };
    bool selected_ { false };
    bool learning_ { false };

    double mouseDownTime_ { 0.0 };
    bool isMouseDown_ { false };
    static constexpr double kLongPressThreshold_ = 500.0;

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LongPressButton)
};
