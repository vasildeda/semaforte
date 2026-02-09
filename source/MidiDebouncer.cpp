#include "MidiDebouncer.h"
#include <optional>

void MidiDebouncer::prepare(double sampleRate, int samplesPerBlock, int ignoreTimeMs)
{
    samplesPerBlock_ = samplesPerBlock;
    ignoreSamples_ = static_cast<juce::int64>(sampleRate * ignoreTimeMs * 0.001);
    samplesSinceLast_ = ignoreSamples_;
}

std::optional<juce::MidiMessage> MidiDebouncer::processBlock(const juce::MidiBuffer& midi)
{
    for (const auto metadata : midi)
    {
        auto msg = metadata.getMessage();

        // Skip Note Off and Note On with velocity 0
        if (msg.isNoteOff() || (msg.isNoteOn() && msg.getVelocity() == 0))
            continue;

        int samplePos = metadata.samplePosition;

        // accumulate sample counter across blocks
        juce::int64 samplesElapsed = samplesSinceLast_ + samplePos;

        if (samplesElapsed >= ignoreSamples_)
        {
            samplesSinceLast_ = 0; // reset after accepting message
            return msg;
        }
    }

    // nothing allowed this block
    samplesSinceLast_ += midi.getNumEvents() > 0 ? midi.getLastEventTime() : samplesPerBlock_;
    return std::nullopt;
}
