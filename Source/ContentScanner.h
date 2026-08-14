#pragma once
#include <JuceHeader.h>

struct UcgContentItem
{
    enum class Type { audio, sfz, milanSampleBank, milanPatchBank,
                      hypersonicPreset, nexusPreset, unknown };
    juce::File file;
    juce::String name;
    juce::String bankId;
    juce::StringArray presetNames;
    Type type = Type::unknown;
    bool playable = false;
};

class ContentScanner
{
public:
    static UcgContentItem inspect(const juce::File& file);
    static juce::Array<UcgContentItem> scan(const juce::File& root);
    static juce::String typeName(UcgContentItem::Type type);
};
