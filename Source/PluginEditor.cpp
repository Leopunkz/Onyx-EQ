#include "PluginEditor.h"

namespace {
    const juce::Colour panel(0xff252a2c), dark(0xff15191a), green(0xffa7c64b), cream(0xffe4e2d8);
}
OnyxEQAudioProcessorEditor::OnyxEQAudioProcessorEditor(OnyxEQAudioProcessor& p)
: AudioProcessorEditor(&p), processor(p)
{
    setSize(720, 390);
    brand.setText("ONYX", juce::dontSendNotification);
    brand.setFont(juce::Font(juce::FontOptions(34.0f, juce::Font::bold)));
    brand.setColour(juce::Label::textColourId, cream);
    addAndMakeVisible(brand);
    subtitle.setText("4-BAND CHANNEL EQUALIZER  •  INSPIRED BY THE ONYX 4•BUS", juce::dontSendNotification);
    subtitle.setFont(juce::Font(juce::FontOptions(11.0f, juce::Font::bold)));
    subtitle.setColour(juce::Label::textColourId, green);
    addAndMakeVisible(subtitle);

    styleKnob(low,"LOW"," dB");
    styleKnob(lm,"LOW MID"," dB");
    styleKnob(lmf,"LM FREQ"," Hz");
    styleKnob(hm,"HIGH MID"," dB");
    styleKnob(hmf,"HM FREQ"," Hz");
    styleKnob(high,"HIGH"," dB");

    auto& state=processor.parameters;
    aLow=std::make_unique<SliderAttachment>(state,"low",low);
    aLm=std::make_unique<SliderAttachment>(state,"lm",lm);
    aLmf=std::make_unique<SliderAttachment>(state,"lmf",lmf);
    aHm=std::make_unique<SliderAttachment>(state,"hm",hm);
    aHmf=std::make_unique<SliderAttachment>(state,"hmf",hmf);
    aHigh=std::make_unique<SliderAttachment>(state,"high",high);
    bypass.setButtonText("EQ OUT  /  BYPASS");
    bypass.setColour(juce::ToggleButton::textColourId,cream);
    bypass.setColour(juce::ToggleButton::tickColourId,green);
    bypass.setColour(juce::ToggleButton::tickDisabledColourId,juce::Colours::grey);
    addAndMakeVisible(bypass);
    aBypass=std::make_unique<ButtonAttachment>(state,"bypass",bypass);
}
void OnyxEQAudioProcessorEditor::styleKnob(juce::Slider& s,const juce::String& name,const juce::String& suffix)
{
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow,false,78,20);
    s.setColour(juce::Slider::rotarySliderFillColourId,green);
    s.setColour(juce::Slider::rotarySliderOutlineColourId,juce::Colour(0xff596164));
    s.setColour(juce::Slider::thumbColourId,cream);
    s.setColour(juce::Slider::textBoxTextColourId,cream);
    s.setColour(juce::Slider::textBoxOutlineColourId,juce::Colours::transparentBlack);
    s.setColour(juce::Slider::textBoxBackgroundColourId,dark);
    s.setName(name);
    s.setTextValueSuffix(" " + suffix);
    addAndMakeVisible(s);
}
void OnyxEQAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(dark);
    g.setColour(panel); g.fillRoundedRectangle(12,12,getWidth()-24,getHeight()-24,9);
    g.setColour(juce::Colour(0xff41494b)); g.drawRoundedRectangle(12,12,getWidth()-24,getHeight()-24,9,1.2f);
    g.setColour(green); g.fillRect(28,77,getWidth()-56,2);
    g.setColour(juce::Colour(0xff343b3d)); g.fillRoundedRectangle(27,100,getWidth()-54,230,6);
    g.setColour(cream); g.setFont(juce::Font(juce::FontOptions(11.0f,juce::Font::bold)));
    const juce::String labels[] = { "LOW", "LOW MID", "LM FREQ", "HIGH MID", "HM FREQ", "HIGH" };
    const int xs[] = { 38, 153, 268, 383, 498, 613 };
    const int ws[] = { 105, 105, 105, 105, 105, 80 };
    for (int i=0;i<6;++i) g.drawText(labels[i], xs[i], 103, ws[i], 18, juce::Justification::centred);
    g.setColour(juce::Colour(0xff899397)); g.setFont(juce::Font(juce::FontOptions(10.0f,juce::Font::bold)));
    g.drawText("PERKINS-STYLE EQ  |  FIXED BELL WIDTH",30,342,getWidth()-60,16,juce::Justification::centred);
}
void OnyxEQAudioProcessorEditor::resized()
{
    brand.setBounds(30,22,150,45);
    subtitle.setBounds(190,34,490,25);
    const int y=116, h=190, w=105;
    low.setBounds(38,y,w,h);
    lm.setBounds(153,y,w,h);
    lmf.setBounds(268,y,w,h);
    hm.setBounds(383,y,w,h);
    hmf.setBounds(498,y,w,h);
    high.setBounds(613,y,w-25,h);
    bypass.setBounds(30,350,190,22);
}
