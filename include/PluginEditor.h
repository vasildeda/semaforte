/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "BinaryData.h"
#include "LongPressButton.h"
#include "PluginProcessor.h"

//==============================================================================
class Logo : public juce::Component
{
public:
    Logo()
    {
        logo_ = juce::Drawable::createFromImageData(BinaryData::logo_svg,
                                                    BinaryData::logo_svgSize);
    }

    void paint(juce::Graphics& g) override
    {
        if (logo_)
            logo_->drawWithin(g, getLocalBounds().toFloat().reduced(4),
                              juce::RectanglePlacement::centred, 1.0f);
    }

private:
    std::unique_ptr<juce::Drawable> logo_;
};

//==============================================================================
class SwitchanderAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    SwitchanderAudioProcessorEditor(SwitchanderAudioProcessor&);
    ~SwitchanderAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    SwitchanderAudioProcessor& audioProcessor_;

    Logo logo_;
    std::array<LongPressButton, 5> channelButtons_;

    void updateChannelButtons();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SwitchanderAudioProcessorEditor)
};
