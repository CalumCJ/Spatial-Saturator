/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SpatialSaturatorFilter.h"

//==============================================================================
/**
*/
class SpatialSaturatorAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    SpatialSaturatorAudioProcessor();
    ~SpatialSaturatorAudioProcessor() override;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================

    std::atomic<float>* m_midGain = nullptr;
    std::atomic<float>* m_midFreq = nullptr;
    std::atomic<float>* m_sideGain = nullptr;
    std::atomic<float>* m_sideFreqLower = nullptr;
    std::atomic<float>* m_sideFreqUpper = nullptr;
    std::atomic<float>* m_makeUpGain = nullptr;
    std::atomic<float>* m_tanhAmplitude = nullptr;
    std::atomic<float>* m_tanhSlope = nullptr;
    std::atomic<float>* m_saturatorMix = nullptr;
    std::atomic<float>* m_sinAmplitude = nullptr;
    std::atomic<float>* m_sinFreq = nullptr;

private:

    juce::AudioProcessorValueTreeState m_state;

    double m_sampleRate{};

    void processMidsSides(juce::AudioBuffer<float>&, juce::MidiBuffer&, int numberSamples);
    void processLeftRight(juce::AudioBuffer<float>&, juce::MidiBuffer&, int numberSamples);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpatialSaturatorAudioProcessor)

    MidShelfFilter midShelfFilter;
    SideShelfFilter sideShelfFilter;
    SideHpFilter sideHpFilter;
};
