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
    addAndMakeVisible(switchButton_);

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
        juce::GridItem(switchButton_),
        juce::GridItem(midiLabels_[0]),
        juce::GridItem(midiLabels_[1]),
        juce::GridItem(midiLabels_[2]),
        juce::GridItem(midiLabels_[3]),
        juce::GridItem(midiLabels_[4])
    };

    grid.performLayout(getLocalBounds());
}
