
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

namespace
{
    constexpr double fixedQ = 0.80;

    double dbToAmp(double db)
    {
        return std::pow(10.0, db / 40.0);
    }

    void makePeak(OnyxEQAudioProcessor::Biquad& f,
                  double sr, double hz, double db, double q)
    {
        const double A = dbToAmp(db);
        const double w = 2.0 * juce::MathConstants<double>::pi * hz / sr;
        const double c = std::cos(w);
        const double s = std::sin(w);
        const double alpha = s / (2.0 * q);

        const double a0 = 1.0 + alpha / A;

        f.b0 = (1.0 + alpha * A) / a0;
        f.b1 = (-2.0 * c) / a0;
        f.b2 = (1.0 - alpha * A) / a0;
        f.a1 = (-2.0 * c) / a0;
        f.a2 = (1.0 - alpha / A) / a0;
    }

    void makeShelf(OnyxEQAudioProcessor::Biquad& f,
                   double sr, double hz, double db, bool high)
    {
        const double A = dbToAmp(db);
        const double w = 2.0 * juce::MathConstants<double>::pi * hz / sr;
        const double c = std::cos(w);
        const double s = std::sin(w);

        const double alpha = (s / 2.0) * std::sqrt(2.0);
        const double beta = 2.0 * std::sqrt(A) * alpha;

        double b0, b1, b2, a0, a1, a2;

        if (!high)
        {
            b0 = A * ((A+1)-(A-1)*c+beta);
            b1 = 2*A * ((A-1)-(A+1)*c);
            b2 = A * ((A+1)-(A-1)*c-beta);

            a0 = (A+1)+(A-1)*c+beta;
            a1 = -2*((A-1)+(A+1)*c);
            a2 = (A+1)+(A-1)*c-beta;
        }
        else
        {
            b0 = A * ((A+1)+(A-1)*c+beta);
            b1 = -2*A * ((A-1)+(A+1)*c);
            b2 = A * ((A+1)+(A-1)*c-beta);

            a0 = (A+1)-(A-1)*c+beta;
            a1 = 2*((A-1)-(A+1)*c);
            a2 = (A+1)-(A-1)*c-beta;
        }

        f.b0 = b0/a0;
        f.b1 = b1/a0;
        f.b2 = b2/a0;
        f.a1 = a1/a0;
        f.a2 = a2/a0;
    }

    // High-pass Butterworth del terzo ordine:
    // 18 dB per ottava, frequenza di taglio 100 Hz.

    void makeFirstOrderHighPass(
        OnyxEQAudioProcessor::Biquad& f,
        double sr, double hz)
    {
        const double k = std::tan(
            juce::MathConstants<double>::pi * hz / sr);

        const double norm = 1.0 / (1.0 + k);

        f.b0 = norm;
        f.b1 = -norm;
        f.b2 = 0.0;

        f.a1 = (k - 1.0) * norm;
        f.a2 = 0.0;
    }

    void makeSecondOrderHighPass(
        OnyxEQAudioProcessor::Biquad& f,
        double sr, double hz, double q)
    {
        const double w =
            2.0 * juce::MathConstants<double>::pi * hz / sr;

        const double c = std::cos(w);
        const double s = std::sin(w);
        const double alpha = s / (2.0 * q);

        const double a0 = 1.0 + alpha;

        f.b0 = ((1.0 + c) * 0.5) / a0;
        f.b1 = (-(1.0 + c)) / a0;
        f.b2 = ((1.0 + c) * 0.5) / a0;

        f.a1 = (-2.0 * c) / a0;
        f.a2 = (1.0 - alpha) / a0;
    }
}

OnyxEQAudioProcessor::OnyxEQAudioProcessor()
    : AudioProcessor(
        BusesProperties()
            .withInput("Input", juce::AudioChannelSet::stereo(), true)
            .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout
OnyxEQAudioProcessor::createParameterLayout()
{
    using APF = juce::AudioParameterFloat;

    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;

    p.push_back(std::make_unique<APF>(
        "low", "LOW",
        juce::NormalisableRange<float>(-15, 15, 0.1f), 0));

    p.push_back(std::make_unique<APF>(
        "lm", "LOW MID",
        juce::NormalisableRange<float>(-15, 15, 0.1f), 0));

    p.push_back(std::make_unique<APF>(
        "lmf", "LOW MID FREQ",
        juce::NormalisableRange<float>(100, 2000, 1, 0.35f), 400));

    p.push_back(std::make_unique<APF>(
        "hm", "HIGH MID",
        juce::NormalisableRange<float>(-15, 15, 0.1f), 0));

    p.push_back(std::make_unique<APF>(
        "hmf", "HIGH MID FREQ",
        juce::NormalisableRange<float>(400, 8000, 1, 0.35f), 2500));

    p.push_back(std::make_unique<APF>(
        "high", "HIGH",
        juce::NormalisableRange<float>(-15, 15, 0.1f), 0));

    p.push_back(std::make_unique<juce::AudioParameterBool>(
        "lowcut", "LOW CUT", false));

    p.push_back(std::make_unique<juce::AudioParameterBool>(
        "bypass", "EQ OUT", false));

    return { p.begin(), p.end() };
}

bool OnyxEQAudioProcessor::isBusesLayoutSupported(
    const BusesLayout& layouts) const
{
    const auto in = layouts.getMainInputChannelSet();
    const auto out = layouts.getMainOutputChannelSet();

    return in == out &&
        (out == juce::AudioChannelSet::mono() ||
         out == juce::AudioChannelSet::stereo());
}

void OnyxEQAudioProcessor::prepareToPlay(
    double sampleRate, int)
{
    currentSampleRate = sampleRate;

    for (int c = 0; c < 2; ++c)
    {
        lowShelf[c].reset();
        lowMid[c].reset();
        highMid[c].reset();
        highShelf[c].reset();

        lowCutFirst[c].reset();
        lowCutSecond[c].reset();
    }

    updateFilters();
}

void OnyxEQAudioProcessor::updateFilters()
{
    const auto val = [this](const char* id)
    {
        return parameters.getRawParameterValue(id)->load();
    };

    const double lmF = juce::jlimit(
        100.0, currentSampleRate * 0.45,
        (double) val("lmf"));

    const double hmF = juce::jlimit(
        400.0, currentSampleRate * 0.45,
        (double) val("hmf"));

    for (int c = 0; c < 2; ++c)
    {
        makeShelf(lowShelf[c], currentSampleRate,
                  80.0, val("low"), false);

        makePeak(lowMid[c], currentSampleRate,
                 lmF, val("lm"), fixedQ);

        makePeak(highMid[c], currentSampleRate,
                 hmF, val("hm"), fixedQ);

        makeShelf(highShelf[c], currentSampleRate,
                  12000.0, val("high"), true);

        makeFirstOrderHighPass(
            lowCutFirst[c], currentSampleRate, 100.0);

        makeSecondOrderHighPass(
            lowCutSecond[c], currentSampleRate, 100.0, 1.0);
    }
}

void OnyxEQAudioProcessor::processBlock(
    juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    updateFilters();

    const bool lowCutOn =
        parameters.getRawParameterValue("lowcut")->load() > 0.5f;

    const bool eqBypassed =
        parameters.getRawParameterValue("bypass")->load() > 0.5f;

    const int channels =
        juce::jmin(2, getTotalNumInputChannels());

    for (int c = 0; c < channels; ++c)
    {
        auto* d = buffer.getWritePointer(c);

        for (int n = 0; n < buffer.getNumSamples(); ++n)
        {
            float x = d[n];

            if (lowCutOn)
            {
                x = lowCutFirst[c].process(x);
                x = lowCutSecond[c].process(x);
            }

            if (!eqBypassed)
            {
                x = lowShelf[c].process(x);
                x = lowMid[c].process(x);
                x = highMid[c].process(x);
                x = highShelf[c].process(x);
            }

            d[n] = x;
        }
    }

    for (int c = getTotalNumInputChannels();
         c < getTotalNumOutputChannels(); ++c)
    {
        buffer.clear(c, 0, buffer.getNumSamples());
    }
}

juce::AudioProcessorEditor*
OnyxEQAudioProcessor::createEditor()
{
    return new OnyxEQAudioProcessorEditor(*this);
}

void OnyxEQAudioProcessor::getStateInformation(
    juce::MemoryBlock& dest)
{
    auto state = parameters.copyState();

    std::unique_ptr<juce::XmlElement> xml(state.createXml());

    copyXmlToBinary(*xml, dest);
}

void OnyxEQAudioProcessor::setStateInformation(
    const void* data, int size)
{
    std::unique_ptr<juce::XmlElement> xml(
        getXmlFromBinary(data, size));

    if (xml && xml->hasTagName(parameters.state.getType()))
    {
        parameters.replaceState(
            juce::ValueTree::fromXml(*xml));
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OnyxEQAudioProcessor();
}
