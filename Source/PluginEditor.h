#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class UCGInfinity16XAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                                  public juce::FileDragAndDropTarget
{
public:
    explicit UCGInfinity16XAudioProcessorEditor(UCGInfinity16XAudioProcessor&);
    void paint(juce::Graphics&) override;
    void resized() override;
    bool isInterestedInFileDrag(const juce::StringArray&) override { return true; }
    void filesDropped(const juce::StringArray&, int, int) override;
private:
    UCGInfinity16XAudioProcessor& processor;
    juce::Label title, preset, status;
    juce::TextButton scanButton { "SCAN CONTENT" };
    juce::ListBox parts;
    class PartModel final : public juce::ListBoxModel
    {
    public:
        explicit PartModel(UCGInfinity16XAudioProcessor& p) : proc(p) {}
        int getNumRows() override { return 16; }
        void paintListBoxItem(int row, juce::Graphics& g, int w, int h, bool selected) override;
    private: UCGInfinity16XAudioProcessor& proc;
    } model;
    void inspect(const juce::File&);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UCGInfinity16XAudioProcessorEditor)
};
