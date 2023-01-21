/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <math.h>

//==============================================================================
SpatialSaturatorAudioProcessor::SpatialSaturatorAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
#endif
    m_state(*this, nullptr, "midandsideParams", createParameterLayout())
{
    m_midGain = m_state.getRawParameterValue("midGainID");
    m_midFreq = m_state.getRawParameterValue("midFreqID");
    m_sideGain = m_state.getRawParameterValue("sideGainID");
    m_sideFreqLower = m_state.getRawParameterValue("sideFreqLowerID");
    m_sideFreqUpper = m_state.getRawParameterValue("sideFreqUpperID");
    m_tanhAmplitude = m_state.getRawParameterValue("tanhAmplitudeID");
    m_tanhSlope = m_state.getRawParameterValue("tanhSlopeID");
    m_saturatorMix = m_state.getRawParameterValue("saturatorMixID");
    m_sinAmplitude = m_state.getRawParameterValue("sinAmplitudeID");
    m_sinFreq = m_state.getRawParameterValue("sinFreqID");
    m_makeUpGain = m_state.getRawParameterValue("makeUpGainID");
}

SpatialSaturatorAudioProcessor::~SpatialSaturatorAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout SpatialSaturatorAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    auto midGain = std::make_unique<juce::AudioParameterFloat>("midGainID", "mid Gain (dB)", juce::NormalisableRange<float>(0.0f, 12.0f, 0.5f), 2.5f);
    params.push_back(std::move(midGain));

    auto midFreq = std::make_unique<juce::AudioParameterFloat>("midFreqID", "mid Freq (Hz)", juce::NormalisableRange<float>(20.0f, 1000.0f, 1.0f), 250.0f);
    params.push_back(std::move(midFreq));

    auto sideGain = std::make_unique<juce::AudioParameterFloat>("sideGainID", "side Gain (dB)", juce::NormalisableRange<float>(0.0f, 12.0f, 0.5f), 6.0f);
    params.push_back(std::move(sideGain));

    auto sideFreqLower = std::make_unique<juce::AudioParameterFloat>("sideFreqLowerID", " side Freq Lower (Hz)", juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f), 140.0f);
    params.push_back(std::move(sideFreqLower));

    auto sideFreqUpper = std::make_unique<juce::AudioParameterFloat>("sideFreqUpperID", "side Freq Upper (Hz)", juce::NormalisableRange<float>(1000.0f, 20000.0f, 1.0f), 4000.0f);
    params.push_back(std::move(sideFreqUpper));

    auto tanhAmp = std::make_unique<juce::AudioParameterFloat>("tanhAmplitudeID", "Tanh Amplitude", juce::NormalisableRange<float>(0.5f, 100.0f, 1.0f), 50.0f);
    params.push_back(std::move(tanhAmp));

    auto tanhSlope = std::make_unique<juce::AudioParameterFloat>("tanhSlopeID", "Tanh Slope", juce::NormalisableRange<float>(1.0f, 15.0f, 1.0f), 7.0f);
    params.push_back(std::move(tanhSlope));

    auto saturatorMix = std::make_unique<juce::AudioParameterFloat>("saturatorMixID", "Saturator Mix (d/w)", juce::NormalisableRange<float>(1.0f, 100.0f, 1.0f), 50.0f);
    params.push_back(std::move(saturatorMix));

    auto sinAmp = std::make_unique<juce::AudioParameterFloat>("sinAmplitudeID", "Sin Amplitude", juce::NormalisableRange<float>(0.5f, 100.0f, 1.0f), 50.0f);
    params.push_back(std::move(sinAmp));

    auto sinFreq = std::make_unique<juce::AudioParameterFloat>("sinFrequencyID", "Sin Frequency (Hz)", juce::NormalisableRange<float>(0.5f, 100.0f, 1.0f), 60.0f);
    params.push_back(std::move(sinFreq));

    auto makeUpGain = std::make_unique<juce::AudioParameterFloat>("makeUpGainID", "Make Up Gain (dB)", juce::NormalisableRange<float>(-12.0f, 12.0f, 0.5f), 0.0f);
    params.push_back(std::move(makeUpGain));

    return { params.begin(), params.end() };
}

//==============================================================================
const juce::String SpatialSaturatorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SpatialSaturatorAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool SpatialSaturatorAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool SpatialSaturatorAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double SpatialSaturatorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SpatialSaturatorAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int SpatialSaturatorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SpatialSaturatorAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String SpatialSaturatorAudioProcessor::getProgramName(int index)
{
    return {};
}

void SpatialSaturatorAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void SpatialSaturatorAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    // Set sample rate for all filter instances
    midShelfFilter.setSampleRate(static_cast<float>(sampleRate));
    sideShelfFilter.setSampleRate(static_cast<float>(sampleRate));
    sideHpFilter.setSampleRate(static_cast<float>(sampleRate));

}

void SpatialSaturatorAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SpatialSaturatorAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const juce::AudioChannelSet& mainInput = layouts.getMainInputChannelSet();
    const juce::AudioChannelSet& mainOutput = layouts.getMainOutputChannelSet();

    return mainInput.size() == 2 && mainOutput.size() == 2;
}
#endif

void SpatialSaturatorAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Find buffer length
    const int numSamples = buffer.getNumSamples();
    if (numSamples == 0)
    {
        return; // Exit if no samples
    }

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Two Output channels
    auto channelDataL = buffer.getWritePointer(0);
    auto channelDataR = buffer.getWritePointer(1);

    // Process L+R into mids & sides
    processMidsSides(buffer, midiMessages, numSamples);

    // Apply mid low shelf to mids
    midShelfFilter.processFilter(buffer, midiMessages, numSamples, m_midFreq, m_midGain);

    // Apply side high pass to sides
    sideHpFilter.processFilter(buffer, midiMessages, numSamples, m_sideFreqLower);

    // Apply side low-shelf to sides 
    sideShelfFilter.processFilter(buffer, midiMessages, numSamples, m_sideFreqUpper, m_sideGain);

    // Process mids+sides back to L&R
    processLeftRight(buffer, midiMessages, numSamples);

    double input;

    // Waveshaper Saturator and Limiter
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    {
        for (auto n = numSamples; n < numSamples; ++n)
        {
            input = (double)buffer.getSample(i, n);

            // waveshaper saturator 
            input = ((double)*m_tanhAmplitude*0.01) * tanh(input * (double)*m_tanhSlope) + ((double)*m_sinAmplitude*0.01) * sin(input * (double)*m_sinFreq);

            // Mixer Processing
            input = input * (double)*m_saturatorMix;

            // Limiter (To be developed)

        }
    }
}

//==============================================================================
bool SpatialSaturatorAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SpatialSaturatorAudioProcessor::createEditor()
{
    return new SpatialSaturatorAudioProcessorEditor(*this, m_state);
}

//==============================================================================
void SpatialSaturatorAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = m_state.copyState();
    std::unique_ptr<juce::XmlElement>xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void SpatialSaturatorAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(m_state.state.getType()))
            m_state.replaceState(juce::ValueTree::fromXml(*xmlState));
}

void SpatialSaturatorAudioProcessor::processMidsSides(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages, int numberSamples)
{
    for (int n = 0; n < numberSamples; ++n)
    {
        // process mids
        double mids = ((double)buffer.getSample(0, n) + (double)buffer.getSample(1, n)) / 2;
        // process sides
        double sides = ((double)buffer.getSample(0, n) - (double)buffer.getSample(1, n)) / 2;

        // Set output
        buffer.setSample(0, n, mids);
        buffer.setSample(1, n, sides);

    }
}

void SpatialSaturatorAudioProcessor::processLeftRight(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages, int numberSamples)
{
    for (int n = 0; n < numberSamples; ++n)
    {
        // process left
        double left = ((double)buffer.getSample(0, n) + (double)buffer.getSample(1, n)) * juce::Decibels::decibelsToGain((float)*m_makeUpGain);
        // process right
        double right = ((double)buffer.getSample(0, n) - (double)buffer.getSample(1, n)) * juce::Decibels::decibelsToGain((float)*m_makeUpGain);

        // Set output
        buffer.setSample(0, n, left);
        buffer.setSample(1, n, right);
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SpatialSaturatorAudioProcessor();
}
