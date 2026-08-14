#pragma once
#include <JuceHeader.h>

struct UcgContentItem
{
    enum class Type { audio, sfz, hypersonicPreset, nexusPreset, unknown };
    juce::File file;
    juce::String name;
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
