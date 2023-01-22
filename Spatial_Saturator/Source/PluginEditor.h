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

    // Mid Gain Slider
    juce::Slider midGainSlider;
    juce::Label midGainSliderLabel;
    std::unique_ptr<SliderAttachment> midGainSliderAttachment;

    // Mid Frequency Slider
    juce::Slider midFreqSlider;
    juce::Label midFreqSliderLabel;
    std::unique_ptr<SliderAttachment> midFreqSliderAttachment;

    // Side Gain Slider
    juce::Slider sideGainSlider;
    juce::Label sideGainSliderLabel;
    std::unique_ptr<SliderAttachment> sideGainSliderAttachment;

    // Side Frequency Lower Slider
    juce::Slider sideFreqLowerSlider;
    juce::Label sideFreqLowerSliderLabel;
    std::unique_ptr<SliderAttachment> sideFreqLowerSliderAttachment;

    // Side Frequency Upper Slider
    juce::Slider sideFreqUpperSlider;
    juce::Label sideFreqUpperSliderLabel;
    std::unique_ptr<SliderAttachment> sideFreqUpperSliderAttachment;

    // Tanh amplitude Slider
    juce::Slider tanhAmplitudeSlider;
    juce::Label tanhAmplitudeSliderLabel;
    std::unique_ptr<SliderAttachment> tanhAmplitudeSliderAttachment;

    // Tanh slope Slider
    juce::Slider tanhSlopeSlider;
    juce::Label tanhSlopeSliderLabel;
    std::unique_ptr<SliderAttachment> tanhSlopeSliderAttachment;

    // Saturator Mix Slider
    juce::Slider saturatorMixSlider;
    juce::Label saturatorMixSliderLabel;
    std::unique_ptr<SliderAttachment> saturatorMixSliderAttachment;

    // Sin Amplitude Slider
    juce::Slider sinAmplitudeSlider;
    juce::Label sinAmplitudeSliderLabel;
    std::unique_ptr<SliderAttachment> sinAmplitudeSliderAttachment;

    // Sin Frequency Slider
    juce::Slider sinFrequencySlider;
    juce::Label sinFrequencySliderLabel;
    std::unique_ptr<SliderAttachment> sinFrequencySliderAttachment;

    // Make Up Gain Slider
    juce::Slider makeUpGainSlider;
    juce::Label makeUpGainSliderLabel;
    std::unique_ptr<SliderAttachment> makeUpGainSliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpatialSaturatorAudioProcessorEditor)
};
