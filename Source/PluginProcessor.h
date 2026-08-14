#pragma once
#include <JuceHeader.h>
#include "ContentScanner.h"

class UCGInfinity16XAudioProcessor final : public juce::AudioProcessor
{
public:
    UCGInfinity16XAudioProcessor();
    ~UCGInfinity16XAudioProcessor() override = default;
    void prepareToPlay(double, int) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout&) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "UCG Infinity 16X"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 2.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return "UCG Multi"; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    bool loadAudioToPart(int part, const juce::File& file);
    juce::StringArray partNames;
    std::array<bool, 16> partEnabled {};

private:
    juce::AudioFormatManager formats;
    std::array<juce::Synthesiser, 16> synths;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UCGInfinity16XAudioProcessor)
};
