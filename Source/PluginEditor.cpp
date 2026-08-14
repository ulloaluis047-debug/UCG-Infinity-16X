#include "PluginEditor.h"

UCGInfinity16XAudioProcessorEditor::UCGInfinity16XAudioProcessorEditor(UCGInfinity16XAudioProcessor& p)
  : AudioProcessorEditor(&p), processor(p), model(p)
{
    setSize(1200, 760);
    title.setText("UCG INFINITY 16X", juce::dontSendNotification);
    title.setFont(juce::Font(34.0f, juce::Font::bold));
    title.setColour(juce::Label::textColourId, juce::Colour(0xff65f4ff));
    preset.setText("ROMANTIC GRAND", juce::dontSendNotification);
    preset.setFont(juce::Font(46.0f, juce::Font::bold));
    preset.setJustificationType(juce::Justification::centred);
    status.setText("ARRASTRA LIBRERÍAS AQUÍ", juce::dontSendNotification);
    status.setJustificationType(juce::Justification::centred);
    status.setColour(juce::Label::textColourId, juce::Colour(0xff78ff9f));
    parts.setModel(&model);
    parts.setColour(juce::ListBox::backgroundColourId, juce::Colour(0xff0b1119));
    scanButton.onClick = [this]
    {
        juce::FileChooser chooser("Selecciona CONTENT", {}, "*");
        if (chooser.browseForDirectory()) inspect(chooser.getResult());
    };
    for (auto* c : std::initializer_list<juce::Component*>{&title,&preset,&status,&scanButton,&parts}) addAndMakeVisible(c);
}

void UCGInfinity16XAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff060a10));
    auto r = getLocalBounds().toFloat().reduced(14.0f);
    g.setColour(juce::Colour(0xff172536)); g.fillRoundedRectangle(r, 14.0f);
    g.setColour(juce::Colour(0xff8c43ff)); g.drawRoundedRectangle(r, 14.0f, 1.5f);
    g.setGradientFill({juce::Colour(0xff071522), 0, 100, juce::Colour(0xff101022), (float)getWidth(), 500, false});
    g.fillRoundedRectangle(300.0f, 100.0f, 870.0f, 230.0f, 12.0f);
    g.setColour(juce::Colour(0xff19dff2)); g.drawRoundedRectangle(320.0f, 235.0f, 830.0f, 75.0f, 10.0f, 2.0f);
}

void UCGInfinity16XAudioProcessorEditor::resized()
{
    title.setBounds(35, 25, 520, 50);
    preset.setBounds(320, 115, 830, 75);
    status.setBounds(330, 245, 810, 55);
    scanButton.setBounds(35, 100, 230, 42);
    parts.setBounds(35, 170, 1130, 550);
}

void UCGInfinity16XAudioProcessorEditor::PartModel::paintListBoxItem(int row, juce::Graphics& g, int w, int h, bool selected)
{
    if (selected || row == 0) g.fillAll(row == 0 ? juce::Colour(0xff093849) : juce::Colour(0xff24184a));
    g.setColour(row == 0 ? juce::Colour(0xff65f4ff) : juce::Colours::white);
    g.setFont(16.0f);
    g.drawText(juce::String(row + 1).paddedLeft('0', 2) + "   " + proc.partNames[row]
               + (proc.partEnabled[(size_t)row] ? "     ● ON" : "     ○ OFF"), 12, 0, w - 20, h, juce::Justification::centredLeft);
}

void UCGInfinity16XAudioProcessorEditor::filesDropped(const juce::StringArray& files, int, int)
{
    if (files.isEmpty()) return;
    inspect(juce::File(files[0]));
}

void UCGInfinity16XAudioProcessorEditor::inspect(const juce::File& target)
{
    if (target.isDirectory())
    {
        const auto items = ContentScanner::scan(target);
        int playable = 0, hyper = 0, nexus = 0;
        for (const auto& i : items)
        {
            playable += i.playable ? 1 : 0;
            hyper += i.type == UcgContentItem::Type::hypersonicPreset ? 1 : 0;
            nexus += i.type == UcgContentItem::Type::nexusPreset ? 1 : 0;
        }
        status.setText(juce::String(items.size()) + " ARCHIVOS · " + juce::String(playable) + " LISTOS · "
                       + juce::String(hyper) + " HYPERSONIC · " + juce::String(nexus) + " NEXUS",
                       juce::dontSendNotification);
        return;
    }
    auto item = ContentScanner::inspect(target);
    if (item.playable && processor.loadAudioToPart(1, target))
        status.setText(item.name + " — CARGADO EN PART 2", juce::dontSendNotification);
    else
        status.setText(item.name + " — " + ContentScanner::typeName(item.type), juce::dontSendNotification);
    parts.repaint();
}
