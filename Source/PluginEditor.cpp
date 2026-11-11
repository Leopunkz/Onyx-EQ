#include "PluginEditor.h"
#include <cmath>

namespace
{
    const juce::Colour panel = juce::Colour(0xff202426);
    const juce::Colour panelDark = juce::Colour(0xff111517);
    const juce::Colour metal = juce::Colour(0xff777e80);
    const juce::Colour blue = juce::Colour(0xff087eae);
    const juce::Colour blueLight = juce::Colour(0xff43b4dc);
    const juce::Colour silver = juce::Colour(0xffa1a6a7);
    const juce::Colour silverLight = juce::Colour(0xffd1d4d2);
    const juce::Colour white = juce::Colour(0xffeeeeea);

    constexpr float startAngle =
        juce::MathConstants<float>::pi * 1.25f;

    constexpr float endAngle =
        juce::MathConstants<float>::pi * 2.75f;
}

// --------------------------------------------------
// CUSTOM KNOB
// --------------------------------------------------

void OnyxLookAndFeel::drawRotarySlider(
    juce::Graphics& g,
    int x, int y, int width, int height,
    float sliderPosProportional,
    float rotaryStartAngle,
    float rotaryEndAngle,
    juce::Slider& slider)
{
    const float size =
        static_cast<float>(juce::jmin(width, height));

    auto bounds = juce::Rectangle<float>(
        static_cast<float>(x),
        static_cast<float>(y),
        size, size).reduced(13.0f);

    const auto centre = bounds.getCentre();
    const float radius = bounds.getWidth() * 0.5f;

    const bool blueKnob =
        slider.getComponentID() == "blue-knob";

    const auto faceColour =
        blueKnob ? blue : silver;

    const auto highlight =
        blueKnob ? blueLight : silverLight;

    // Graduated scale

    for (int i = 0; i <= 20; ++i)
    {
        const float proportion = i / 20.0f;

        const float angle =
            rotaryStartAngle +
            proportion * (rotaryEndAngle - rotaryStartAngle);

        const float outer = radius + 1.0f;
        const float inner =
            radius - (i % 5 == 0 ? 9.0f : 5.0f);

        const juce::Point<float> p1(
            centre.x + inner * std::cos(angle),
            centre.y + inner * std::sin(angle));

        const juce::Point<float> p2(
            centre.x + outer * std::cos(angle),
            centre.y + outer * std::sin(angle));

        g.setColour(
            i % 5 == 0 ? white : white.withAlpha(0.55f));

        g.drawLine(
            p1.x, p1.y,
            p2.x, p2.y,
            i % 5 == 0 ? 2.0f : 1.0f);
    }

    // Outer shadow

    g.setColour(panelDark);
    g.fillEllipse(bounds);

    // Metal ring

    g.setGradientFill(
        juce::ColourGradient(
            silverLight,
            bounds.getTopLeft(),
            panelDark,
            bounds.getBottomRight(),
            false));

    g.fillEllipse(bounds.reduced(4.0f));

    // Inner dark ring

    auto inner = bounds.reduced(9.0f);

    g.setColour(panelDark);
    g.fillEllipse(inner);

    // Knob face

    auto face = bounds.reduced(14.0f);

    g.setGradientFill(
        juce::ColourGradient(
            highlight,
            face.getTopLeft(),
            faceColour.darker(0.65f),
            face.getBottomRight(),
            false));

    g.fillEllipse(face);

    g.setColour(highlight.withAlpha(0.45f));
    g.drawEllipse(face.reduced(2.0f), 1.0f);

    // Position indicator

    const float angle =
        rotaryStartAngle +
        sliderPosProportional *
        (rotaryEndAngle - rotaryStartAngle);

    juce::Path pointer;

    pointer.addRoundedRectangle(
        -3.0f,
        -radius * 0.68f,
        6.0f,
        radius * 0.48f,
        2.5f);

    g.setColour(white);

    g.fillPath(
        pointer,
        juce::AffineTransform::rotation(angle)
            .translated(centre.x, centre.y));

    g.setColour(panelDark);

    g.fillEllipse(
        centre.x - 4.0f,
        centre.y - 4.0f,
        8.0f, 8.0f);
}

// --------------------------------------------------
// EDITOR CONSTRUCTOR
// --------------------------------------------------

OnyxEQAudioProcessorEditor::OnyxEQAudioProcessorEditor(
    OnyxEQAudioProcessor& p)
    : AudioProcessorEditor(&p),
      processor(p)
{
    setLookAndFeel(&onyxLookAndFeel);

    styleKnob(high, "HIGH", true, true);
    styleKnob(hm, "HIGH MID GAIN", false, true);
    styleKnob(hmf, "HIGH MID FREQ", true, false);
    styleKnob(lm, "LOW MID GAIN", false, true);
    styleKnob(lmf, "LOW MID FREQ", true, false);
    styleKnob(low, "LOW", false, true);

    addAndMakeVisible(high);
    addAndMakeVisible(hm);
    addAndMakeVisible(hmf);
    addAndMakeVisible(lm);
    addAndMakeVisible(lmf);
    addAndMakeVisible(low);

    // Buttons

    lowCut.setButtonText("LOW CUT   100 Hz");
    bypass.setButtonText("EQ BYPASS");

    lowCut.setClickingTogglesState(true);
    bypass.setClickingTogglesState(true);

    lowCut.setColour(
        juce::ToggleButton::textColourId, white);

    lowCut.setColour(
        juce::ToggleButton::tickColourId, blueLight);

    bypass.setColour(
        juce::ToggleButton::textColourId, white);

    bypass.setColour(
        juce::ToggleButton::tickColourId, blueLight);

    addAndMakeVisible(lowCut);
    addAndMakeVisible(bypass);

    // Connections to the audio processor

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

    // Brand

    brand.setText(
        "MACKIE",
        juce::dontSendNotification);

    brand.setFont(
        juce::Font(27.0f, juce::Font::bold));

    brand.setColour(
        juce::Label::textColourId, white);

    brand.setJustificationType(
        juce::Justification::centredLeft);

    addAndMakeVisible(brand);

    // Model

    subtitle.setText(
        "ONYX 32.4",
        juce::dontSendNotification);

    subtitle.setFont(
        juce::Font(22.0f, juce::Font::bold));

    subtitle.setColour(
        juce::Label::textColourId, white);

    subtitle.setJustificationType(
        juce::Justification::centredRight);

    addAndMakeVisible(subtitle);

    setSize(430, 940);
}

// --------------------------------------------------
// DESTRUCTOR
// --------------------------------------------------

OnyxEQAudioProcessorEditor::~OnyxEQAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

// --------------------------------------------------
// KNOB CONFIGURATION
// --------------------------------------------------

void OnyxEQAudioProcessorEditor::styleKnob(
    juce::Slider& knob,
    const juce::String& name,
    bool blueKnob,
    bool gainKnob)
{
    knob.setName(name);

    knob.setComponentID(
        blueKnob ? "blue-knob" : "silver-knob");

    knob.setSliderStyle(
        juce::Slider::RotaryHorizontalVerticalDrag);

    knob.setRotaryParameters(
        startAngle, endAngle, true);

    knob.setTextBoxStyle(
        juce::Slider::TextBoxBelow,
        false, 76, 20);

    knob.setColour(
        juce::Slider::textBoxTextColourId, white);

    knob.setColour(
        juce::Slider::textBoxBackgroundColourId,
        panelDark);

    knob.setColour(
        juce::Slider::textBoxOutlineColourId,
        metal);

    knob.setNumDecimalPlacesToDisplay(
        gainKnob ? 1 : 0);

    knob.setTextValueSuffix(
        gainKnob ? " dB" : " Hz");
}

// --------------------------------------------------
// BACKGROUND AND LABELS
// --------------------------------------------------

void OnyxEQAudioProcessorEditor::paint(
    juce::Graphics& g)
{
    g.fillAll(panel);

    auto area = getLocalBounds().toFloat();

    // Outer panel

    g.setColour(panelDark);

    g.fillRoundedRectangle(
        area.reduced(7.0f), 6.0f);

    g.setColour(metal);

    g.drawRoundedRectangle(
        area.reduced(7.0f), 6.0f, 2.0f);

    // Header line

    g.setColour(blue);

    g.fillRect(20.0f, 65.0f, 390.0f, 3.0f);

    // Main title

    g.setColour(white);

    g.setFont(
        juce::Font(22.0f, juce::Font::bold));

    g.drawText(
        "CHANNEL EQ",
        20, 75, 390, 32,
        juce::Justification::centred);

    // Band names

    g.setFont(
        juce::Font(18.0f, juce::Font::bold));

    g.drawText(
        "HIGH",
        25, 145, 100, 30,
        juce::Justification::centredLeft);

    g.drawText(
        "HIGH MID",
        25, 320, 110, 30,
        juce::Justification::centredLeft);

    g.drawText(
        "LOW MID",
        25, 505, 110, 30,
        juce::Justification::centredLeft);

    g.drawText(
        "LOW",
        25, 690, 100, 30,
        juce::Justification::centredLeft);

    // Control descriptions

    g.setFont(12.0f);
    g.setColour(silverLight);

    g.drawText(
        "GAIN",
        135, 260, 100, 18,
        juce::Justification::centred);

    g.drawText(
        "GAIN",
        110, 440, 100, 18,
        juce::Justification::centred);

    g.drawText(
        "FREQ",
        275, 440, 100, 18,
        juce::Justification::centred);

    g.drawText(
        "GAIN",
        110, 625, 100, 18,
        juce::Justification::centred);

    g.drawText(
        "FREQ",
        275, 625, 100, 18,
        juce::Justification::centred);

    g.drawText(
        "GAIN",
        165, 810, 100, 18,
        juce::Justification::centred);

    // Horizontal separators

    g.setColour(blue.withAlpha(0.85f));

    g.drawLine(20, 285, 410, 285, 1.5f);
    g.drawLine(20, 470, 410, 470, 1.5f);
    g.drawLine(20, 655, 410, 655, 1.5f);
    g.drawLine(20, 840, 410, 840, 1.5f);

    // Vertical separators

    g.setColour(blue.withAlpha(0.65f));

    g.drawLine(245, 305, 245, 455, 1.0f);
    g.drawLine(245, 490, 245, 640, 1.0f);
}

// --------------------------------------------------
// CONTROL POSITIONS
// --------------------------------------------------

void OnyxEQAudioProcessorEditor::resized()
{
    brand.setBounds(25, 15, 180, 40);
    subtitle.setBounds(220, 15, 185, 40);

    high.setBounds(155, 110, 135, 150);

    hm.setBounds(105, 300, 135, 150);
    hmf.setBounds(265, 300, 135, 150);

    lm.setBounds(105, 485, 135, 150);
    lmf.setBounds(265, 485, 135, 150);

    low.setBounds(155, 670, 135, 150);

    lowCut.setBounds(35, 855, 220, 40);
    bypass.setBounds(265, 855, 130, 40);
}
