#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class OnyxEQAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit OnyxEQAudioProcessorEditor(OnyxEQAudioProcessor&);
    ~OnyxEQAudioProcessorEditor() override = default;
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    OnyxEQAudioProcessor& processor;
    juce::Slider low, lm, lmf, hm, hmf, high;
    juce::ToggleButton bypass;
    std::unique_ptr<SliderAttachment> aLow,aLm,aLmf,aHm,aHmf,aHigh;
    std::unique_ptr<ButtonAttachment> aBypass;
    juce::Label brand, subtitle;
    void styleKnob(juce::Slider&, const juce::String&, const juce::String&);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OnyxEQAudioProcessorEditor)
};
