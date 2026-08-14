#include "ContentScanner.h"

namespace
{
juce::String readFourCC(juce::FileInputStream& in, juce::int64 offset)
{
    char bytes[5] {};
    in.setPosition(offset);
    if (in.read(bytes, 4) != 4) return {};
    return juce::String::fromUTF8(bytes, 4);
}

juce::String fxpName(juce::FileInputStream& in)
{
    char bytes[29] {};
    in.setPosition(28);
    in.read(bytes, 28);
    return juce::String::fromUTF8(bytes, 28).trimCharactersAtEnd("\0 ");
}
}

UcgContentItem ContentScanner::inspect(const juce::File& file)
{
    UcgContentItem item { file, file.getFileNameWithoutExtension() };
    const auto ext = file.getFileExtension().toLowerCase();
    if (ext == ".wav" || ext == ".aif" || ext == ".aiff" || ext == ".flac")
    {
        item.type = UcgContentItem::Type::audio;
        item.playable = true;
        return item;
    }
    if (ext == ".sfz")
    {
        item.type = UcgContentItem::Type::sfz;
        item.playable = false;
        return item;
    }
    if (ext != ".fxp") return item;

    juce::FileInputStream in(file);
    if (!in.openedOk() || readFourCC(in, 0) != "CcnK") return item;
    const auto pluginId = readFourCC(in, 16);
    item.name = fxpName(in);
    if (pluginId == "StSS") item.type = UcgContentItem::Type::hypersonicPreset;
    if (pluginId == "NEXU") item.type = UcgContentItem::Type::nexusPreset;
    // FXP stores the original engine state, not its copyrighted sample content.
    item.playable = false;
    return item;
}

juce::Array<UcgContentItem> ContentScanner::scan(const juce::File& root)
{
    juce::Array<UcgContentItem> result;
    juce::Array<juce::File> files;
    root.findChildFiles(files, juce::File::findFiles, true);
    for (const auto& file : files)
    {
        auto item = inspect(file);
        if (item.type != UcgContentItem::Type::unknown) result.add(item);
    }
    return result;
}

juce::String ContentScanner::typeName(UcgContentItem::Type type)
{
    switch (type)
    {
        case UcgContentItem::Type::audio: return "AUDIO — LISTO";
        case UcgContentItem::Type::sfz: return "SFZ — IMPORTADOR REQUERIDO";
        case UcgContentItem::Type::hypersonicPreset: return "HYPERSONIC FXP — CONTENIDO REQUERIDO";
        case UcgContentItem::Type::nexusPreset: return "NEXUS 2 FXP — CONTENIDO REQUERIDO";
        default: return "NO RECONOCIDO";
    }
}
