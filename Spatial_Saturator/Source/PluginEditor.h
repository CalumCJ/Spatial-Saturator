/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SpatialSaturatorFilter.h"

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

//==============================================================================
/**
*/
class SpatialSaturatorAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    SpatialSaturatorAudioProcessorEditor(SpatialSaturatorAudioProcessor&, juce::AudioProcessorValueTreeState& vts);
    ~SpatialSaturatorAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SpatialSaturatorAudioProcessor& audioProcessor;

    // Reference to Value Tree State object
    juce::AudioProcessorValueTreeState& treeState;

    // mid Gain Slider
    juce::Slider midGainSlider;
    juce::Label midGainSliderLabel;
    std::unique_ptr<SliderAttachment> midGainSliderAttachment;

    // mid Frequency Slider
    juce::Slider midFreqSlider;
    juce::Label midFreqSliderLabel;
    std::unique_ptr<SliderAttachment> midFreqSliderAttachment;

    // side Gain Slider
    juce::Slider sideGainSlider;
    juce::Label sideGainSliderLabel;
    std::unique_ptr<SliderAttachment> sideGainSliderAttachment;

    // side Frequency Lower Slider
    juce::Slider sideFreqLowerSlider;
    juce::Label sideFreqLowerSliderLabel;
    std::unique_ptr<SliderAttachment> sideFreqLowerSliderAttachment;

    // side Frequency Upper Slider
    juce::Slider sideFreqUpperSlider;
    juce::Label sideFreqUpperSliderLabel;
    std::unique_ptr<SliderAttachment> sideFreqUpperSliderAttachment;

    // Make Up Gain Slider
    juce::Slider makeUpGainSlider;
    juce::Label makeUpGainSliderLabel;
    std::unique_ptr<SliderAttachment> makeUpGainSliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpatialSaturatorAudioProcessorEditor)
};
