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

bool readMilanHeader(juce::FileInputStream& in, juce::String& bankId)
{
    char header[48] {};
    in.setPosition(0);
    if (in.read(header, 47) != 47) return false;
    const auto text = juce::String::fromUTF8(header, 47);
    if (!text.startsWith("Milan Bank file ")) return false;
    bankId = text.substring(16, 22).trim();
    return bankId.length() >= 5;
}

juce::StringArray extractMilanPatchNames(juce::FileInputStream& in)
{
    const auto bytesToRead = (int) juce::jmin<juce::int64>(in.getTotalLength(), 1024 * 1024);
    juce::MemoryBlock data((size_t) bytesToRead, true);
    in.setPosition(0);
    in.read(data.getData(), bytesToRead);
    const auto* b = static_cast<const juce::uint8*>(data.getData());
    juce::StringArray names;
    for (int i = 48; i + 4 < bytesToRead; ++i)
    {
        const int len = b[i];
        if (len < 4 || len > 63 || i + 1 + len > bytesToRead) continue;
        bool printable = true;
        for (int n = 0; n < len; ++n)
            printable = printable && b[i + 1 + n] >= 32 && b[i + 1 + n] <= 126;
        if (!printable) continue;
        const auto candidate = juce::String::fromUTF8(reinterpret_cast<const char*>(b + i + 1), len).trim();
        if (candidate.length() >= 4 && !candidate.containsIgnoreCase("HS-2")
            && !candidate.containsChar('=') && !names.contains(candidate))
            names.add(candidate);
        i += len;
    }
    return names;
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
    juce::FileInputStream in(file);
    if (!in.openedOk()) return item;
    if (readMilanHeader(in, item.bankId))
    {
        item.type = item.bankId.startsWithChar('S') ? UcgContentItem::Type::milanSampleBank
                                                    : UcgContentItem::Type::milanPatchBank;
        if (item.type == UcgContentItem::Type::milanPatchBank)
            item.presetNames = extractMilanPatchNames(in);
        item.name = file.getFileName();
        item.playable = false;
        return item;
    }
    if (ext != ".fxp" || readFourCC(in, 0) != "CcnK") return item;
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
        case UcgContentItem::Type::milanSampleBank: return "MILAN SAMPLE BANK — RECONOCIDO";
        case UcgContentItem::Type::milanPatchBank: return "MILAN PATCH BANK — RECONOCIDO";
        case UcgContentItem::Type::hypersonicPreset: return "HYPERSONIC FXP — CONTENIDO REQUERIDO";
        case UcgContentItem::Type::nexusPreset: return "NEXUS 2 FXP — CONTENIDO REQUERIDO";
        default: return "NO RECONOCIDO";
    }
}
