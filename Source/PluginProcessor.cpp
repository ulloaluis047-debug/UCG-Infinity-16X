#include "PluginProcessor.h"
#include "PluginEditor.h"

UCGInfinity16XAudioProcessor::UCGInfinity16XAudioProcessor()
  : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    formats.registerBasicFormats();
    for (int i = 0; i < 16; ++i)
    {
        synths[(size_t) i].addVoice(new juce::SamplerVoice());
        synths[(size_t) i].addVoice(new juce::SamplerVoice());
        partNames.add(i == 0 ? "ROMANTIC GRAND — LOAD SAMPLE" : "EMPTY PART " + juce::String(i + 1));
        partEnabled[(size_t) i] = false;
    }
}

void UCGInfinity16XAudioProcessor::prepareToPlay(double rate, int)
{
    for (auto& synth : synths) synth.setCurrentPlaybackSampleRate(rate);
}

bool UCGInfinity16XAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void UCGInfinity16XAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals guard;
    buffer.clear();
    juce::AudioBuffer<float> layer(buffer.getNumChannels(), buffer.getNumSamples());
    for (int i = 0; i < 16; ++i)
    {
        if (!partEnabled[(size_t) i]) continue;
        layer.clear();
        synths[(size_t) i].renderNextBlock(layer, midi, 0, layer.getNumSamples());
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            buffer.addFrom(ch, 0, layer, ch, 0, layer.getNumSamples());
    }
    buffer.applyGain(0.25f);
}

bool UCGInfinity16XAudioProcessor::loadAudioToPart(int part, const juce::File& file)
{
    if (!juce::isPositiveAndBelow(part, 16)) return false;
    std::unique_ptr<juce::AudioFormatReader> reader(formats.createReaderFor(file));
    if (reader == nullptr) return false;
    juce::BigInteger notes; notes.setRange(0, 128, true);
    auto sound = new juce::SamplerSound(file.getFileNameWithoutExtension(), *reader, notes, 60, 0.01, 0.15, 30.0);
    synths[(size_t) part].clearSounds();
    synths[(size_t) part].addSound(sound);
    partNames.set(part, file.getFileNameWithoutExtension());
    partEnabled[(size_t) part] = true;
    return true;
}

void UCGInfinity16XAudioProcessor::getStateInformation(juce::MemoryBlock& dest)
{
    juce::ValueTree state("UCG_INFINITY");
    for (int i = 0; i < 16; ++i)
    {
        state.setProperty("name" + juce::String(i), partNames[i], nullptr);
        state.setProperty("enabled" + juce::String(i), partEnabled[(size_t)i], nullptr);
    }
    if (auto xml = state.createXml()) copyXmlToBinary(*xml, dest);
}

void UCGInfinity16XAudioProcessor::setStateInformation(const void* data, int size)
{
    if (auto xml = getXmlFromBinary(data, size))
    {
        auto state = juce::ValueTree::fromXml(*xml);
        for (int i = 0; i < 16; ++i)
        {
            partNames.set(i, state.getProperty("name" + juce::String(i), partNames[i]).toString());
            partEnabled[(size_t)i] = state.getProperty("enabled" + juce::String(i), i == 0);
        }
    }
}

juce::AudioProcessorEditor* UCGInfinity16XAudioProcessor::createEditor() { return new UCGInfinity16XAudioProcessorEditor(*this); }
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new UCGInfinity16XAudioProcessor(); }
