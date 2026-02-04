#include "MidiDebouncer.h"
#include <optional>

void MidiDebouncer::prepare(double sampleRate, int samplesPerBlock, int ignoreTimeMs)
{
    samplesPerBlock_ = samplesPerBlock;
    ignoreSamples_ = static_cast<juce::int64>(sampleRate * ignoreTimeMs * 0.001);
    printf("Ignore samples: %llD\n", ignoreSamples_);
    samplesSinceLast_ = ignoreSamples_;
}

std::optional<juce::MidiMessage> MidiDebouncer::processBlock(const juce::MidiBuffer& midi)
{
    for (const auto metadata : midi)
    {
        int samplePos = metadata.samplePosition;

        // accumulate sample counter across blocks
        juce::int64 samplesElapsed = samplesSinceLast_ + samplePos;

        if (samplesElapsed >= ignoreSamples_)
        {
            printf("Samples elapsed: %lld\n", samplesElapsed);
            samplesSinceLast_ = 0; // reset after accepting message
            return metadata.getMessage();
        }
    }

    // nothing allowed this block
    samplesSinceLast_ += midi.getNumEvents() > 0 ? midi.getLastEventTime() : samplesPerBlock_;
    return std::nullopt;
}
