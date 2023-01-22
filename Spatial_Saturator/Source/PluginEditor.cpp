/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SpatialSaturatorAudioProcessorEditor::SpatialSaturatorAudioProcessorEditor(SpatialSaturatorAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&p), audioProcessor(p), treeState(vts)
{
    midGainSlider.setTextValueSuffix(" dB ");
    addAndMakeVisible(midGainSlider);
    midGainSliderAttachment.reset(new SliderAttachment(treeState, "midGainID", midGainSlider));
    addAndMakeVisible(midGainSliderLabel);
    midGainSliderLabel.setText("mid Gain", juce::dontSendNotification);
    midGainSliderLabel.attachToComponent(&midGainSlider, true);

    midFreqSlider.setTextValueSuffix(" Hz ");
    addAndMakeVisible(midFreqSlider);
    midFreqSliderAttachment.reset(new SliderAttachment(treeState, "midFreqID", midFreqSlider));
    addAndMakeVisible(midFreqSliderLabel);
    midFreqSliderLabel.setText("mid Frequency", juce::dontSendNotification);
    midFreqSliderLabel.attachToComponent(&midFreqSlider, true);

    sideGainSlider.setTextValueSuffix(" dB ");
    addAndMakeVisible(sideGainSlider);
    sideGainSliderAttachment.reset(new SliderAttachment(treeState, "sideGainID", sideGainSlider));
    addAndMakeVisible(sideGainSliderLabel);
    sideGainSliderLabel.setText("side Gain", juce::dontSendNotification);
    sideGainSliderLabel.attachToComponent(&sideGainSlider, true);

    sideFreqLowerSlider.setTextValueSuffix(" Hz ");
    addAndMakeVisible(sideFreqLowerSlider);
    sideFreqLowerSliderAttachment.reset(new SliderAttachment(treeState, "sideFreqLowerID", sideFreqLowerSlider));
    addAndMakeVisible(sideFreqLowerSliderLabel);
    sideFreqLowerSliderLabel.setText("Lower side Frequency", juce::dontSendNotification);
    sideFreqLowerSliderLabel.attachToComponent(&sideFreqLowerSlider, true);

    sideFreqUpperSlider.setTextValueSuffix(" Hz ");
    addAndMakeVisible(sideFreqUpperSlider);
    sideFreqUpperSliderAttachment.reset(new SliderAttachment(treeState, "sideFreqUpperID", sideFreqUpperSlider));
    addAndMakeVisible(sideFreqUpperSliderLabel);
    sideFreqUpperSliderLabel.setText("Higher side Frequency", juce::dontSendNotification);
    sideFreqUpperSliderLabel.attachToComponent(&sideFreqUpperSlider, true);

    tanhAmplitudeSlider.setTextValueSuffix(" ");
    addAndMakeVisible(tanhAmplitudeSlider);
    tanhSlopeSliderAttachment.reset(new SliderAttachment(treeState, "tanhAmplitudeID", tanhAmplitudeSlider));
    addAndMakeVisible(tanhAmplitudeSliderLabel);
    tanhAmplitudeSliderLabel.setText("Tanh Amplitude", juce::dontSendNotification);
    tanhAmplitudeSliderLabel.attachToComponent(&tanhAmplitudeSlider, true);

    tanhSlopeSlider.setTextValueSuffix(" g ");
    addAndMakeVisible(tanhSlopeSlider);
    tanhAmplitudeSliderAttachment.reset(new SliderAttachment(treeState, "tanhSlopeID", tanhSlopeSlider));
    addAndMakeVisible(tanhSlopeSliderLabel);
    tanhSlopeSliderLabel.setText("Tanh Slope", juce::dontSendNotification);
    tanhSlopeSliderLabel.attachToComponent(&tanhSlopeSlider, true);

    saturatorMixSlider.setTextValueSuffix(" ");
    addAndMakeVisible(saturatorMixSlider);
    saturatorMixSliderAttachment.reset(new SliderAttachment(treeState, "saturatorMixID", saturatorMixSlider));
    addAndMakeVisible(saturatorMixSliderLabel);
    saturatorMixSliderLabel.setText("Saturation Mix", juce::dontSendNotification);
    saturatorMixSliderLabel.attachToComponent(&saturatorMixSlider, true);

    sinAmplitudeSlider.setTextValueSuffix(" ");
    addAndMakeVisible(sinAmplitudeSlider);
    sinAmplitudeSliderAttachment.reset(new SliderAttachment(treeState, "sinAmplitudeID", sinAmplitudeSlider));
    addAndMakeVisible(sinAmplitudeSliderLabel);
    sinAmplitudeSliderLabel.setText("Sin Amplitude", juce::dontSendNotification);
    sinAmplitudeSliderLabel.attachToComponent(&sinAmplitudeSlider, true);

    sinFrequencySlider.setTextValueSuffix(" Hz ");
    addAndMakeVisible(sinFrequencySlider);
    sinFrequencySliderAttachment.reset(new SliderAttachment(treeState, "sinFreqID", sinAmplitudeSlider));
    addAndMakeVisible(sinFrequencySliderLabel);
    sinFrequencySliderLabel.setText("Sin Frequency", juce::dontSendNotification);
    sinFrequencySliderLabel.attachToComponent(&sinFrequencySlider, true);

    makeUpGainSlider.setTextValueSuffix(" dB ");
    addAndMakeVisible(makeUpGainSlider);
    makeUpGainSliderAttachment.reset(new SliderAttachment(treeState, "makeUpGainID", makeUpGainSlider));
    addAndMakeVisible(makeUpGainSliderLabel);
    makeUpGainSliderLabel.setText("Make Up Gain", juce::dontSendNotification);
    makeUpGainSliderLabel.attachToComponent(&makeUpGainSlider, true);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(800, 600);
}

SpatialSaturatorAudioProcessorEditor::~SpatialSaturatorAudioProcessorEditor()
{
}

//==============================================================================
void SpatialSaturatorAudioProcessorEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

}

void SpatialSaturatorAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    auto sliderLeft = 250;
    auto sliderHeight = 20;

    int numSliders = 11;
    int N = 1;
    int step = (getHeight() / (numSliders + 1));

    midGainSlider.setBounds(sliderLeft, (N++ * step) - (sliderHeight / 2), getWidth() - sliderLeft * 1.5, sliderHeight);
    midFreqSlider.setBounds(sliderLeft, (N++ * step) - (sliderHeight / 2), getWidth() - sliderLeft * 1.5, sliderHeight);
    sideGainSlider.setBounds(sliderLeft, (N++ * step) - (sliderHeight / 2), getWidth() - sliderLeft * 1.5, sliderHeight);
    sideFreqLowerSlider.setBounds(sliderLeft, (N++ * step) - (sliderHeight / 2), getWidth() - sliderLeft * 1.5, sliderHeight);
    sideFreqUpperSlider.setBounds(sliderLeft, (N++ * step) - (sliderHeight / 2), getWidth() - sliderLeft * 1.5, sliderHeight);
    tanhAmplitudeSlider.setBounds(sliderLeft, (N++ * step) - (sliderHeight / 2), getWidth() - sliderLeft * 1.5, sliderHeight);
    tanhSlopeSlider.setBounds(sliderLeft, (N++ * step) - (sliderHeight / 2), getWidth() - sliderLeft * 1.5, sliderHeight);
    saturatorMixSlider.setBounds(sliderLeft, (N++ * step) - (sliderHeight / 2), getWidth() - sliderLeft * 1.5, sliderHeight);
    sinAmplitudeSlider.setBounds(sliderLeft, (N++ * step) - (sliderHeight / 2), getWidth() - sliderLeft * 1.5, sliderHeight);
    sinFrequencySlider.setBounds(sliderLeft, (N++ * step) - (sliderHeight / 2), getWidth() - sliderLeft * 1.5, sliderHeight);
    makeUpGainSlider.setBounds(sliderLeft, (N++ * step) - (sliderHeight / 2), getWidth() - sliderLeft * 1.5, sliderHeight);
}
