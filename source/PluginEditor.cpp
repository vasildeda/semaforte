/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SwitchanderAudioProcessorEditor::SwitchanderAudioProcessorEditor(SwitchanderAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor_(p)
{
    addAndMakeVisible(logo_);

    for (int i = 0; i < 5; ++i)
    {
        channelButtons_[i].setText("--");
        channelButtons_[i].onClick = [this, i] {
            audioProcessor_.selectBus(i);
        };
        channelButtons_[i].onLongPress = [this, i] {
            audioProcessor_.midiLearnTarget_.store(i, std::memory_order_relaxed);
            updateChannelButtons();
        };
        addAndMakeVisible(channelButtons_[i]);
    }

    // Register callback for processor → GUI updates
    audioProcessor_.onStateChanged = [this] {
        updateChannelButtons();
    };

    // Initial state
    updateChannelButtons();

    setSize(704, 396);
}

SwitchanderAudioProcessorEditor::~SwitchanderAudioProcessorEditor()
{
    audioProcessor_.onStateChanged = nullptr;
}

void SwitchanderAudioProcessorEditor::updateChannelButtons()
{
    int selectedBus = audioProcessor_.selectedBus_.load(std::memory_order_relaxed);
    int learningBus = audioProcessor_.midiLearnTarget_.load(std::memory_order_relaxed);

    for (int i = 0; i < 5; ++i)
    {
        channelButtons_[i].setSelected(i == selectedBus);
        channelButtons_[i].setLearning(i == learningBus);

        // Update text based on trigger assignment
        int32_t trigger = audioProcessor_.getMidiTrigger(i);
        if (trigger < 0)
        {
            channelButtons_[i].setText("--");
        }
        else
        {
            // Display as hex for now (status:data1)
            int status = (trigger >> 8) & 0xFF;
            int data1 = trigger & 0xFF;
            channelButtons_[i].setText(juce::String::toHexString(status) + ":" +
                                       juce::String::toHexString(data1));
        }
    }
}

//==============================================================================
void SwitchanderAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void SwitchanderAudioProcessorEditor::resized()
{
    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;

    constexpr float gap = 8.0f;

    juce::Grid grid;
    grid.templateRows = { Track(Fr(1)), Track(Fr(1)) };
    grid.templateColumns = { Track(Fr(1)), Track(Fr(1)), Track(Fr(1)),
                             Track(Fr(1)), Track(Fr(1)) };
    grid.columnGap = juce::Grid::Px(gap);
    grid.rowGap = juce::Grid::Px(gap);

    auto marginLeft = juce::GridItem::Margin(0, 0, gap, gap);    // bottom, left
    auto marginMiddle = juce::GridItem::Margin(0, 0, gap, 0);    // bottom only
    auto marginRight = juce::GridItem::Margin(0, gap, gap, 0);   // bottom, right

    grid.items = {
        juce::GridItem(logo_).withColumn({ 2, 5 }).withRow({ 1, 2 }),  // span columns 2-4
        juce::GridItem(channelButtons_[0]).withColumn({ 1, 2 }).withRow({ 2, 3 }).withMargin(marginLeft),
        juce::GridItem(channelButtons_[1]).withColumn({ 2, 3 }).withRow({ 2, 3 }).withMargin(marginMiddle),
        juce::GridItem(channelButtons_[2]).withColumn({ 3, 4 }).withRow({ 2, 3 }).withMargin(marginMiddle),
        juce::GridItem(channelButtons_[3]).withColumn({ 4, 5 }).withRow({ 2, 3 }).withMargin(marginMiddle),
        juce::GridItem(channelButtons_[4]).withColumn({ 5, 6 }).withRow({ 2, 3 }).withMargin(marginRight)
    };

    grid.performLayout(getLocalBounds());
}
