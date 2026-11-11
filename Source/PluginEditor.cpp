
#include "PluginEditor.h"

namespace
{
    const juce::Colour background(0xff111719);
    const juce::Colour panel(0xff292f31);
    const juce::Colour inset(0xff1c2224);
    const juce::Colour line(0xff596366);
    const juce::Colour blue(0xff3979b9);
    const juce::Colour cream(0xffe5dfc9);
    const juce::Colour text(0xffd5d9d7);
}

OnyxEQAudioProcessorEditor::OnyxEQAudioProcessorEditor(
    OnyxEQAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(820, 500);

    brand.setText("ONYX", juce::dontSendNotification);
    brand.setFont(juce::Font(
        juce::FontOptions(34.0f, juce::Font::bold)));
    brand.setColour(
        juce::Label::textColourId, cream);
    addAndMakeVisible(brand);

    subtitle.setText(
        "4-BAND CHANNEL EQ  |  DIGITAL MODEL",
        juce::dontSendNotification);

    subtitle.setFont(juce::Font(
        juce::FontOptions(12.0f, juce::Font::bold)));

    subtitle.setColour(
        juce::Label::textColourId, text);

    addAndMakeVisible(subtitle);

    styleKnob(low, "LOW", true);
    styleKnob(lm, "LOW MID", true);
    styleKnob(lmf, "FREQUENCY", false);
    styleKnob(hm, "HIGH MID", true);
    styleKnob(hmf, "FREQUENCY", false);
    styleKnob(high, "HIGH", true);

    auto& state = processor.parameters;

    aLow = std::make_unique<SliderAttachment>(
        state, "low", low);

    aLm = std::make_unique<SliderAttachment>(
        state, "lm", lm);

    aLmf = std::make_unique<SliderAttachment>(
        state, "lmf", lmf);

    aHm = std::make_unique<SliderAttachment>(
        state, "hm", hm);

    aHmf = std::make_unique<SliderAttachment>(
        state, "hmf", hmf);

    aHigh = std::make_unique<SliderAttachment>(
        state, "high", high);

    lowCut.setButtonText("LOW CUT  100 Hz");

    lowCut.setColour(
        juce::ToggleButton::textColourId, cream);

    lowCut.setColour(
        juce::ToggleButton::tickColourId, blue);

    addAndMakeVisible(lowCut);

    aLowCut = std::make_unique<ButtonAttachment>(
        state, "lowcut", lowCut);

    bypass.setButtonText("EQ OUT  /  BYPASS");

    bypass.setColour(
        juce::ToggleButton::textColourId, cream);

    bypass.setColour(
        juce::ToggleButton::tickColourId, blue);

    addAndMakeVisible(bypass);

    aBypass = std::make_unique<ButtonAttachment>(
        state, "bypass", bypass);
}

void OnyxEQAudioProcessorEditor::styleKnob(
    juce::Slider& s,
    const juce::String& name,
    bool gainKnob)
{
    s.setSliderStyle(
        juce::Slider::RotaryHorizontalVerticalDrag);

    s.setTextBoxStyle(
        juce::Slider::TextBoxBelow,
        false, 82, 22);

    s.setColour(
        juce::Slider::rotarySliderFillColourId, blue);

    s.setColour(
        juce::Slider::rotarySliderOutlineColourId, line);

    s.setColour(
        juce::Slider::thumbColourId,
        gainKnob ? blue.brighter(0.25f) : cream);

    s.setColour(
        juce::Slider::textBoxTextColourId, cream);

    s.setColour(
        juce::Slider::textBoxOutlineColourId,
        juce::Colours::transparentBlack);

    s.setColour(
        juce::Slider::textBoxBackgroundColourId, inset);

    s.setName(name);

    if (gainKnob)
        s.setTextValueSuffix(" dB");
    else
        s.setTextValueSuffix(" Hz");

    addAndMakeVisible(s);
}

void OnyxEQAudioProcessorEditor::paint(
    juce::Graphics& g)
{
    g.fillAll(background);

    auto outer = getLocalBounds()
        .toFloat()
        .reduced(12.0f);

    g.setColour(panel);
    g.fillRoundedRectangle(outer, 8.0f);

    g.setColour(line);
    g.drawRoundedRectangle(outer, 8.0f, 1.2f);

    g.setColour(blue);
    g.fillRect(
        28.0f, 82.0f,
        getWidth() - 56.0f, 2.0f);

    auto strip = juce::Rectangle<float>(
        27.0f, 105.0f,
        getWidth() - 54.0f, 300.0f);

    g.setColour(inset);
    g.fillRoundedRectangle(strip, 5.0f);

    g.setColour(line);
    g.drawRoundedRectangle(strip, 5.0f, 1.0f);

    const juce::String labels[] =
    {
        "LOW",
        "LOW MID",
        "LM FREQ",
        "HIGH MID",
        "HM FREQ",
        "HIGH"
    };

    const int xs[] =
    {
        34, 164, 294, 424, 554, 684
    };

    const int ws[] =
    {
        112, 112, 112, 112, 112, 102
    };

    g.setColour(text);

    g.setFont(juce::Font(
        juce::FontOptions(11.0f, juce::Font::bold)));

    for (int i = 0; i < 6; ++i)
    {
        g.drawText(
            labels[i],
            xs[i], 111,
            ws[i], 18,
            juce::Justification::centred);
    }

    g.setColour(cream);

    g.setFont(juce::Font(
        juce::FontOptions(10.0f)));

    g.drawText(
        "80 Hz", 34, 382, 112, 15,
        juce::Justification::centred);

    g.drawText(
        "100 Hz - 2 kHz", 164, 382, 112, 15,
        juce::Justification::centred);

    g.drawText(
        "400 Hz - 8 kHz", 424, 382, 112, 15,
        juce::Justification::centred);

    g.drawText(
        "12 kHz", 684, 382, 102, 15,
        juce::Justification::centred);

    g.setColour(line);

    g.fillRect(
        28.0f, 423.0f,
        getWidth() - 56.0f, 1.0f);

    g.setColour(text);

    g.setFont(juce::Font(
        juce::FontOptions(10.0f, juce::Font::bold)));

    g.drawText(
        "PERKINS-INSPIRED RESPONSE  |  MID Q FIXED (APPROX.)",
        28, 463,
        getWidth() - 56, 16,
        juce::Justification::centred);
}

void OnyxEQAudioProcessorEditor::resized()
{
    brand.setBounds(30, 24, 150, 45);
    subtitle.setBounds(195, 35, 500, 24);

    const int y = 130;
    const int h = 235;
    const int w = 112;

    low.setBounds(34, y, w, h);
    lm.setBounds(164, y, w, h);
    lmf.setBounds(294, y, w, h);
    hm.setBounds(424, y, w, h);
    hmf.setBounds(554, y, w, h);
    high.setBounds(684, y, 102, h);

    lowCut.setBounds(35, 428, 180, 25);
    bypass.setBounds(620, 428, 175, 25);
}
