/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SwichanderAudioProcessorEditor::SwichanderAudioProcessorEditor(SwichanderAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor_(p)
{
    addAndMakeVisible(logo_);

    // Set up MIDI learn button
    midiLearnButton_.setClickingTogglesState(true);
    midiLearnButton_.setToggleState(audioProcessor_.midiLearnActive_.load(), juce::dontSendNotification);
    midiLearnButton_.onClick = [this] {
        audioProcessor_.midiLearnActive_.store(midiLearnButton_.getToggleState());
    };
    addAndMakeVisible(midiLearnButton_);

    // Register callback for processor → GUI updates
    audioProcessor_.onMidiLearnStateChanged = [this] {
        midiLearnButton_.setToggleState(audioProcessor_.midiLearnActive_.load(), juce::dontSendNotification);
    };

    for (int i = 0; i < 5; ++i)
    {
        midiLabels_[i].setText("MIDI " + juce::String(i + 1), juce::dontSendNotification);
        midiLabels_[i].setJustificationType(juce::Justification::centred);
        addAndMakeVisible(midiLabels_[i]);
    }

    setSize(704, 396);
}

SwichanderAudioProcessorEditor::~SwichanderAudioProcessorEditor()
{
    audioProcessor_.onMidiLearnStateChanged = nullptr;
}

//==============================================================================
void SwichanderAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void SwichanderAudioProcessorEditor::resized()
{
    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;

    juce::Grid grid;
    grid.templateRows = { Track(Fr(1)), Track(Fr(1)) };
    grid.templateColumns = { Track(Fr(1)), Track(Fr(1)), Track(Fr(1)),
                             Track(Fr(1)), Track(Fr(1)), Track(Fr(1)) };

    grid.items = {
        juce::GridItem(logo_),
        juce::GridItem(),  // empty
        juce::GridItem(),  // empty
        juce::GridItem(),  // empty
        juce::GridItem(),  // empty
        juce::GridItem(),  // empty
        juce::GridItem(midiLearnButton_),
        juce::GridItem(midiLabels_[0]),
        juce::GridItem(midiLabels_[1]),
        juce::GridItem(midiLabels_[2]),
        juce::GridItem(midiLabels_[3]),
        juce::GridItem(midiLabels_[4])
    };

    grid.performLayout(getLocalBounds());
}
