
#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class OnyxLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(
        juce::Graphics& g,
        int x, int y, int width, int height,
        float sliderPosProportional,
        float rotaryStartAngle,
        float rotaryEndAngle,
        juce::Slider& slider) override;
};

class OnyxEQAudioProcessorEditor
    : public juce::AudioProcessorEditor
{
public:
    explicit OnyxEQAudioProcessorEditor(
        OnyxEQAudioProcessor&);

    ~OnyxEQAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment =
        juce::AudioProcessorValueTreeState::SliderAttachment;

    using ButtonAttachment =
        juce::AudioProcessorValueTreeState::ButtonAttachment;

    OnyxEQAudioProcessor& processor;

    OnyxLookAndFeel onyxLookAndFeel;

    juce::Slider low;
    juce::Slider lm;
    juce::Slider lmf;
    juce::Slider hm;
    juce::Slider hmf;
    juce::Slider high;

    juce::ToggleButton lowCut;
    juce::ToggleButton bypass;

    std::unique_ptr<SliderAttachment> aLow;
    std::unique_ptr<SliderAttachment> aLm;
    std::unique_ptr<SliderAttachment> aLmf;
    std::unique_ptr<SliderAttachment> aHm;
    std::unique_ptr<SliderAttachment> aHmf;
    std::unique_ptr<SliderAttachment> aHigh;

    std::unique_ptr<ButtonAttachment> aLowCut;
    std::unique_ptr<ButtonAttachment> aBypass;

    juce::Label brand;
    juce::Label subtitle;

    void styleKnob(
        juce::Slider&,
        const juce::String&,
        bool gainKnob);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        OnyxEQAudioProcessorEditor)
};
