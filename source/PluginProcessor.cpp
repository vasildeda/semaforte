/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SwichanderAudioProcessor::SwichanderAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor(
         BusesProperties()
             .withInput("Input 1", juce::AudioChannelSet::stereo(), true)
             .withInput("Input 2", juce::AudioChannelSet::stereo(), true)
             .withInput("Input 3", juce::AudioChannelSet::stereo(), true)
             .withInput("Input 4", juce::AudioChannelSet::stereo(), true)
             .withInput("Input 5", juce::AudioChannelSet::stereo(), true)
             .withOutput("Output", juce::AudioChannelSet::stereo(), true)
       )
#endif
{
}

SwichanderAudioProcessor::~SwichanderAudioProcessor()
{
}

//==============================================================================
const juce::String SwichanderAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SwichanderAudioProcessor::acceptsMidi() const
{
    return true;
}

bool SwichanderAudioProcessor::producesMidi() const
{
    return false;
}

bool SwichanderAudioProcessor::isMidiEffect() const
{
    return false;
}

double SwichanderAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SwichanderAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SwichanderAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SwichanderAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String SwichanderAudioProcessor::getProgramName(int index)
{
    return {};
}

void SwichanderAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void SwichanderAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    midiDebouncer_.prepare(sampleRate, samplesPerBlock, 1000);
    crossFader_.prepare(sampleRate, 200);
}

void SwichanderAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SwichanderAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
}
#endif

void SwichanderAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    handleMidi(midiMessages);
    processBuffer(buffer);
}

int32_t SwichanderAudioProcessor::packMidiForMatch(const juce::MidiMessage& msg)
{
    const auto* data = msg.getRawData();
    return (static_cast<int32_t>(data[0]) << 8) | static_cast<int32_t>(data[1]);
}

bool SwichanderAudioProcessor::midiMatches(const juce::MidiMessage& incoming, int32_t stored)
{
    if (stored == kUnassignedTrigger)
        return false;

    // Ignore Note On with velocity 0 (treated as Note Off)
    if (incoming.isNoteOn() && incoming.getVelocity() == 0)
        return false;

    return packMidiForMatch(incoming) == stored;
}

void SwichanderAudioProcessor::handleMidi(const juce::MidiBuffer& midi)
{
    if (auto msg = midiDebouncer_.processBlock(midi))
    {
        int target = midiLearnTarget_.load(std::memory_order_relaxed);
        if (target >= 0 && target < 5)
        {
            midiTriggers_[target].store(packMidiForMatch(*msg), std::memory_order_relaxed);
            midiLearnTarget_.store(-1, std::memory_order_relaxed);
            triggerAsyncUpdate();
        }
        else
        {
            for (int i = 0; i < 5; ++i)
            {
                if (midiMatches(*msg, midiTriggers_[i].load(std::memory_order_relaxed)))
                {
                    crossFader_.requestBus(i);
                    selectedBus_.store(i, std::memory_order_relaxed);
                    triggerAsyncUpdate();
                    break;
                }
            }
        }
    }
}

void SwichanderAudioProcessor::processBuffer(juce::AudioBuffer<float>& buffer)
{
    auto out = getBusBuffer(buffer, false, 0);

    auto currentIn = getBusBuffer(buffer, true, crossFader_.getCurrentBus());
    auto targetIn = getBusBuffer(buffer, true, crossFader_.getTargetBus());

    const int numChannels = juce::jmin(
        currentIn.getNumChannels(),
        targetIn.getNumChannels(),
        out.getNumChannels()
    );
    const int numSamples = out.getNumSamples();

    for (int s = 0; s < numSamples; ++s)
    {
        const float g = crossFader_.getNextValue();

        for (int ch = 0; ch < numChannels; ++ch)
        {
            auto sample = currentIn.getSample(ch, s) * (1.0 - g) + targetIn.getSample(ch, s) * g;
            out.setSample(ch, s, sample);
        }
    }
}

//==============================================================================
bool SwichanderAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SwichanderAudioProcessor::createEditor()
{
    return new SwichanderAudioProcessorEditor(*this);
}

//==============================================================================
void SwichanderAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SwichanderAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
int32_t SwichanderAudioProcessor::getMidiTrigger(int bus) const
{
    if (bus < 0 || bus >= 5)
        return kUnassignedTrigger;
    return midiTriggers_[bus].load(std::memory_order_relaxed);
}

void SwichanderAudioProcessor::selectBus(int bus)
{
    if (bus < 0 || bus >= 5)
        return;
    crossFader_.requestBus(bus);
    selectedBus_.store(bus, std::memory_order_relaxed);
    triggerAsyncUpdate();
}

void SwichanderAudioProcessor::handleAsyncUpdate()
{
    if (onStateChanged)
        onStateChanged();
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SwichanderAudioProcessor();
}
