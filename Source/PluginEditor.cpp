
#include "PluginEditor.h"

namespace
{
    const juce::Colour panelColour  = juce::Colour(0xff252a2d);
    const juce::Colour panelLight   = juce::Colour(0xff41484b);
    const juce::Colour blue         = juce::Colour(0xff2587a5);
    const juce::Colour blueLight    = juce::Colour(0xff55b8d2);
    const juce::Colour cream        = juce::Colour(0xffe7dfc9);
    const juce::Colour dark         = juce::Colour(0xff111517);

    void drawScale(juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        auto cx = bounds.getCentreX();
        auto cy = bounds.getCentreY();
        auto radius = bounds.getWidth() * 0.43f;

        for (int i = 0; i <= 20; ++i)
        {
            auto angle = juce::MathConstants<float>::pi
                * (1.25f + 1.5f * i / 20.0f);

            auto inner = radius - (i % 5 == 0 ? 7.0f : 3.0f);
            auto outer = radius;

            g.setColour(i % 5 == 0 ? cream : cream.withAlpha(0.55f));

            g.drawLine(
                cx + inner * std::cos(angle),
                cy + inner * std::sin(angle),
                cx + outer * std::cos(angle),
                cy + outer * std::sin(angle),
                i % 5 == 0 ? 1.6f : 0.8f);
        }
    }
}

void OnyxLookAndFeel::drawRotarySlider(
    juce::Graphics& g,
    int x, int y, int width, int height,
    float sliderPosProportional,
    float rotaryStartAngle,
    float rotaryEndAngle,
    juce::Slider&)
{
    auto size = static_cast<float>(
        juce::jmin(width, height));

    auto bounds = juce::Rectangle<float>(
        static_cast<float>(x),
        static_cast<float>(y),
        size, size).reduced(9.0f);

    auto centre = bounds.getCentre();
    auto radius = bounds.getWidth() * 0.5f;

    drawScale(g, bounds);

    g.setColour(dark);
    g.fillEllipse(bounds.reduced(5.0f));

    g.setColour(juce::Colour(0xff777f82));
    g.fillEllipse(bounds.reduced(9.0f));

    g.setColour(juce::Colour(0xffd0d3d0));
    g.drawEllipse(bounds.reduced(9.0f), 1.0f);

    auto knob = bounds.reduced(14.0f);

    g.setGradientFill(
        juce::ColourGradient(
            juce::Colour(0xff6d777a),
            knob.getTopLeft(),
            juce::Colour(0xff20282b),
            knob.getBottomRight(),
            false));

    g.fillEllipse(knob);

    g.setColour(juce::Colour(0xffaab2b3).withAlpha(0.5f));
    g.drawEllipse(knob.reduced(2.0f), 1.0f);

    auto angle = rotaryStartAngle
        + sliderPosProportional
        * (rotaryEndAngle - rotaryStartAngle);

    auto pointerLength = radius * 0.69f;

    juce::Path pointer;
    pointer.addRoundedRectangle(
        -2.0f, -pointerLength,
        4.0f, pointerLength * 0.45f,
        1.5f);

    g.setColour(cream);
    g.fillPath(
        pointer,
        juce::AffineTransform::rotation(angle)
            .translated(centre.x, centre.y));

    g.setColour(dark);
    g.fillEllipse(
        centre.x - 4.0f,
        centre.y - 4.0f,
        8.0f, 8.0f);
}

OnyxEQAudioProcessorEditor::OnyxEQAudioProcessorEditor(
    OnyxEQAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setLookAndFeel(&onyxLookAndFeel);

    auto setupKnob = [this](juce::Slider& knob,
                            const juce::String& name,
                            bool gain)
    {
        styleKnob(knob, name, gain);
        addAndMakeVisible(knob);
    };

    setupKnob(low, "LOW", true);
    setupKnob(lm, "LOW MID", true);
    setupKnob(lmf, "FREQ", false);
    setupKnob(hm, "HIGH MID", true);
    setupKnob(hmf, "FREQ", false);
    setupKnob(high, "HIGH", true);

    lowCut.setButtonText("LOW CUT 100 Hz");
    bypass.setButtonText("EQ BYPASS");

    addAndMakeVisible(lowCut);
    addAndMakeVisible(bypass);

    aLow = std::make_unique<SliderAttachment>(
        processor.parameters, "low", low);

    aLm = std::make_unique<SliderAttachment>(
        processor.parameters, "lm", lm);

    aLmf = std::make_unique<SliderAttachment>(
        processor.parameters, "lmf", lmf);

    aHm = std::make_unique<SliderAttachment>(
        processor.parameters, "hm", hm);

    aHmf = std::make_unique<SliderAttachment>(
        processor.parameters, "hmf", hmf);

    aHigh = std::make_unique<SliderAttachment>(
        processor.parameters, "high", high);

    aLowCut = std::make_unique<ButtonAttachment>(
        processor.parameters, "lowcut", lowCut);

    aBypass = std::make_unique<ButtonAttachment>(
        processor.parameters, "bypass", bypass);

    brand.setText("MACKIE", juce::dontSendNotification);
    brand.setFont(juce::Font(24.0f, juce::Font::bold));
    brand.setColour(
        juce::Label::textColourId, cream);
    brand.setJustificationType(
        juce::Justification::centred);

    addAndMakeVisible(brand);

    subtitle.setText(
        "ONYX 32.4  |  CHANNEL EQ",
        juce::dontSendNotification);

    subtitle.setFont(juce::Font(11.0f));
    subtitle.setColour(
        juce::Label::textColourId, blueLight);
    subtitle.setJustificationType(
        juce::Justification::centred);

    addAndMakeVisible(subtitle);

    setSize(330, 720);
}

OnyxEQAudioProcessorEditor::~OnyxEQAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void OnyxEQAudioProcessorEditor::styleKnob(
    juce::Slider& knob,
    const juce::String& name,
    bool gainKnob)
{
    knob.setSliderStyle(
        juce::Slider::RotaryHorizontalVerticalDrag);

    knob.setTextBoxStyle(
        juce::Slider::TextBoxBelow,
        false, 72, 20);

    knob.setColour(
        juce::Slider::textBoxTextColourId, cream);

    knob.setColour(
        juce::Slider::textBoxBackgroundColourId, dark);

    knob.setColour(
        juce::Slider::textBoxOutlineColourId,
        panelLight);

    knob.setName(name);

    knob.setNumDecimalPlacesToDisplay(
        gainKnob ? 1 : 0);

    if (gainKnob)
        knob.setTextValueSuffix(" dB");
    else
        knob.setTextValueSuffix(" Hz");
}

void OnyxEQAudioProcessorEditor::paint(
    juce::Graphics& g)
{
    g.fillAll(panelColour);

    auto area = getLocalBounds().toFloat();

    g.setColour(dark);
    g.fillRoundedRectangle(
        area.reduced(5.0f), 8.0f);

    g.setColour(panelLight);
    g.drawRoundedRectangle(
        area.reduced(5.0f), 8.0f, 2.0f);

    g.setColour(blue);
    g.fillRect(12.0f, 48.0f,
               getWidth() - 24.0f, 3.0f);

    g.setColour(cream);
    g.setFont(11.0f);

    g.drawText("HIGH",
        15, 118, 65, 20,
        juce::Justification::centred);

    g.drawText("HIGH MID",
        15, 226, 65, 20,
        juce::Justification::centred);

    g.drawText("LOW MID",
        15, 392, 65, 20,
        juce::Justification::centred);

    g.drawText("LOW",
        15, 552, 65, 20,
        juce::Justification::centred);

    g.setColour(blue);
    g.drawLine(18.0f, 207.0f,
               312.0f, 207.0f, 1.0f);

    g.drawLine(18.0f, 373.0f,
               312.0f, 373.0f, 1.0f);

    g.drawLine(18.0f, 539.0f,
               312.0f, 539.0f, 1.0f);
}

void OnyxEQAudioProcessorEditor::resized()
{
    brand.setBounds(20, 8, 290, 34);
    subtitle.setBounds(20, 50, 290, 22);

    lowCut.setBounds(25, 76, 280, 28);

    high.setBounds(105, 112, 120, 105);

    hm.setBounds(25, 220, 125, 120);
    hmf.setBounds(180, 220, 125, 120);

    lm.setBounds(25, 385, 125, 120);
    lmf.setBounds(180, 385, 125, 120);

    low.setBounds(105, 545, 120, 105);

    bypass.setBounds(25, 670, 280, 28);
}
