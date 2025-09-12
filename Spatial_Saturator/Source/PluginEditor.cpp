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
    // Set custom look and feel
    setLookAndFeel(&customLookAndFeel);

    // Add section components
    addAndMakeVisible(midSection);
    addAndMakeVisible(sideSection);
    addAndMakeVisible(saturationSection);
    addAndMakeVisible(outputSection);

    // Add level meters
    addAndMakeVisible(inputMeterL);
    addAndMakeVisible(inputMeterR);
    addAndMakeVisible(outputMeterL);
    addAndMakeVisible(outputMeterR);

    // Configure Mid Section Controls
    configureSlider(midGainSlider, midGainSliderLabel, "Gain", " dB");
    midGainSliderAttachment.reset(new SliderAttachment(treeState, "midGainID", midGainSlider));

    configureSlider(midFreqSlider, midFreqSliderLabel, "Frequency", " Hz");
    midFreqSliderAttachment.reset(new SliderAttachment(treeState, "midFreqID", midFreqSlider));

    // Configure Side Section Controls
    configureSlider(sideGainSlider, sideGainSliderLabel, "Gain", " dB");
    sideGainSliderAttachment.reset(new SliderAttachment(treeState, "sideGainID", sideGainSlider));

    configureSlider(sideFreqLowerSlider, sideFreqLowerSliderLabel, "High Pass", " Hz");
    sideFreqLowerSliderAttachment.reset(new SliderAttachment(treeState, "sideFreqLowerID", sideFreqLowerSlider));

    configureSlider(sideFreqUpperSlider, sideFreqUpperSliderLabel, "Shelf Freq", " Hz");
    sideFreqUpperSliderAttachment.reset(new SliderAttachment(treeState, "sideFreqUpperID", sideFreqUpperSlider));

    // Configure Saturation Controls
    configureSlider(tanhAmplitudeSlider, tanhAmplitudeSliderLabel, "Tanh Amt", " %");
    tanhAmplitudeSliderAttachment.reset(new SliderAttachment(treeState, "tanhAmplitudeID", tanhAmplitudeSlider));

    configureSlider(tanhSlopeSlider, tanhSlopeSliderLabel, "Tanh Slope", "");
    tanhSlopeSliderAttachment.reset(new SliderAttachment(treeState, "tanhSlopeID", tanhSlopeSlider));

    configureSlider(saturatorMixSlider, saturatorMixSliderLabel, "Mix", " %");
    saturatorMixSliderAttachment.reset(new SliderAttachment(treeState, "saturatorMixID", saturatorMixSlider));

    configureSlider(sinAmplitudeSlider, sinAmplitudeSliderLabel, "Sine Amt", " %");
    sinAmplitudeSliderAttachment.reset(new SliderAttachment(treeState, "sinAmplitudeID", sinAmplitudeSlider));

    configureSlider(sinFrequencySlider, sinFrequencySliderLabel, "Sine Freq", " Hz");
    sinFrequencySliderAttachment.reset(new SliderAttachment(treeState, "sinFreqID", sinFrequencySlider));

    // Configure Output Control
    configureSlider(makeUpGainSlider, makeUpGainSliderLabel, "Output", " dB");
    makeUpGainSliderAttachment.reset(new SliderAttachment(treeState, "makeUpGainID", makeUpGainSlider));

    // Set editor size
    setSize(900, 600);
    setResizable(false, false);
}

SpatialSaturatorAudioProcessorEditor::~SpatialSaturatorAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void SpatialSaturatorAudioProcessorEditor::configureSlider(juce::Slider& slider, juce::Label& label,
    const juce::String& labelText, const juce::String& suffix,
    bool isRotary)
{
    if (isRotary)
    {
        slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    }
    else
    {
        slider.setSliderStyle(juce::Slider::LinearHorizontal);
        slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 70, 20);
    }

    slider.setTextValueSuffix(suffix);
    slider.setPopupDisplayEnabled(true, true, this);
    slider.setDoubleClickReturnValue(true, slider.getRange().getStart() +
        (slider.getRange().getLength() / 2.0));

    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setFont(juce::Font(LABEL_FONT_SIZE));

    addAndMakeVisible(slider);
    addAndMakeVisible(label);
}

//==============================================================================
void SpatialSaturatorAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Gradient background
    juce::ColourGradient gradient(juce::Colour(0xff1e1e1e), 0, 0,
        juce::Colour(0xff0a0a0a), 0, (float)getHeight(), false);
    g.setGradientFill(gradient);
    g.fillAll();

    // Header area
    auto headerArea = getLocalBounds().removeFromTop(60);
    g.setColour(juce::Colour(0xff2a2a2a));
    g.fillRect(headerArea);

    // Plugin title
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font("Arial", HEADER_FONT_SIZE, juce::Font::bold));
    g.drawText("SPATIAL SATURATOR", headerArea, juce::Justification::centred);

    // Subtle line under header
    g.setColour(juce::Colour(0xff4a9eff));
    g.fillRect(0, headerArea.getBottom() - 2, getWidth(), 2);

    // Labels for meter sections
    g.setColour(juce::Colour(0xffc0c0c0));
    g.setFont(10.0f);
    auto meterArea = getLocalBounds();
    meterArea.removeFromTop(65);

    // Input meters label
    auto inputMeterArea = meterArea.removeFromLeft(60);
    g.drawText("INPUT", inputMeterArea.removeFromTop(20), juce::Justification::centred);

    // Output meters label  
    auto outputMeterArea = meterArea.removeFromRight(60);
    g.drawText("OUTPUT", outputMeterArea.removeFromTop(20), juce::Justification::centred);
}

void SpatialSaturatorAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    // Remove header space
    bounds.removeFromTop(65);

    // Level meters on sides
    auto meterWidth = 15;
    auto meterInset = 10;
    auto meterHeight = bounds.getHeight() - 40;

    // Input meters (left side)
    auto inputMeterArea = bounds.removeFromLeft(60);
    inputMeterArea.removeFromTop(20);
    inputMeterL.setBounds(inputMeterArea.removeFromLeft(meterWidth).reduced(0, meterInset));
    inputMeterR.setBounds(inputMeterArea.removeFromLeft(meterWidth).reduced(0, meterInset));

    // Output meters (right side)
    auto outputMeterArea = bounds.removeFromRight(60);
    outputMeterArea.removeFromTop(20);
    outputMeterR.setBounds(outputMeterArea.removeFromRight(meterWidth).reduced(0, meterInset));
    outputMeterL.setBounds(outputMeterArea.removeFromRight(meterWidth).reduced(0, meterInset));

    // Main content area
    bounds.reduce(MARGIN, MARGIN);

    // Create grid layout
    auto topRow = bounds.removeFromTop(bounds.getHeight() / 2);
    auto bottomRow = bounds;

    // Top row: Mid and Side sections
    auto midBounds = topRow.removeFromLeft(topRow.getWidth() / 2).reduced(5);
    auto sideBounds = topRow.reduced(5);

    midSection.setBounds(midBounds);
    sideSection.setBounds(sideBounds);

    // Bottom row: Saturation and Output sections
    auto saturationBounds = bottomRow.removeFromLeft(bottomRow.getWidth() * 0.75f).reduced(5);
    auto outputBounds = bottomRow.reduced(5);

    saturationSection.setBounds(saturationBounds);
    outputSection.setBounds(outputBounds);

    // Layout controls within sections
    layoutMidSection(midBounds);
    layoutSideSection(sideBounds);
    layoutSaturationSection(saturationBounds);
    layoutOutputSection(outputBounds);
}

void SpatialSaturatorAudioProcessorEditor::layoutMidSection(juce::Rectangle<int> bounds)
{
    bounds.removeFromTop(35); // Remove title space
    bounds.reduce(SECTION_PADDING, SECTION_PADDING);

    auto sliderArea = bounds;
    auto centerX = bounds.getCentreX();

    // Position sliders side by side
    auto leftSlider = sliderArea.removeFromLeft(sliderArea.getWidth() / 2);
    auto rightSlider = sliderArea;

    // Mid Gain
    midGainSliderLabel.setBounds(leftSlider.removeFromTop(20));
    midGainSlider.setBounds(leftSlider.removeFromTop(SLIDER_SIZE).reduced(10, 0));

    // Mid Frequency
    midFreqSliderLabel.setBounds(rightSlider.removeFromTop(20));
    midFreqSlider.setBounds(rightSlider.removeFromTop(SLIDER_SIZE).reduced(10, 0));
}

void SpatialSaturatorAudioProcessorEditor::layoutSideSection(juce::Rectangle<int> bounds)
{
    bounds.removeFromTop(35); // Remove title space
    bounds.reduce(SECTION_PADDING, SECTION_PADDING);

    auto sliderArea = bounds;

    // Three sliders in a row
    auto sliderWidth = sliderArea.getWidth() / 3;
    auto leftSlider = sliderArea.removeFromLeft(sliderWidth);
    auto middleSlider = sliderArea.removeFromLeft(sliderWidth);
    auto rightSlider = sliderArea;

    // Side Gain
    sideGainSliderLabel.setBounds(leftSlider.removeFromTop(20));
    sideGainSlider.setBounds(leftSlider.removeFromTop(SLIDER_SIZE).reduced(5, 0));

    // Side Freq Lower
    sideFreqLowerSliderLabel.setBounds(middleSlider.removeFromTop(20));
    sideFreqLowerSlider.setBounds(middleSlider.removeFromTop(SLIDER_SIZE).reduced(5, 0));

    // Side Freq Upper
    sideFreqUpperSliderLabel.setBounds(rightSlider.removeFromTop(20));
    sideFreqUpperSlider.setBounds(rightSlider.removeFromTop(SLIDER_SIZE).reduced(5, 0));
}

void SpatialSaturatorAudioProcessorEditor::layoutSaturationSection(juce::Rectangle<int> bounds)
{
    bounds.removeFromTop(35); // Remove title space
    bounds.reduce(SECTION_PADDING, SECTION_PADDING);

    auto topRowArea = bounds.removeFromTop(bounds.getHeight() / 2);
    auto bottomRowArea = bounds;

    // Top row - 3 sliders
    auto topSliderWidth = topRowArea.getWidth() / 3;
    auto slot1 = topRowArea.removeFromLeft(topSliderWidth);
    auto slot2 = topRowArea.removeFromLeft(topSliderWidth);
    auto slot3 = topRowArea;

    // Tanh Amplitude
    tanhAmplitudeSliderLabel.setBounds(slot1.removeFromTop(20));
    tanhAmplitudeSlider.setBounds(slot1.removeFromTop(SLIDER_SIZE).reduced(5, 0));

    // Tanh Slope
    tanhSlopeSliderLabel.setBounds(slot2.removeFromTop(20));
    tanhSlopeSlider.setBounds(slot2.removeFromTop(SLIDER_SIZE).reduced(5, 0));

    // Saturator Mix
    saturatorMixSliderLabel.setBounds(slot3.removeFromTop(20));
    saturatorMixSlider.setBounds(slot3.removeFromTop(SLIDER_SIZE).reduced(5, 0));

    // Bottom row - 2 sliders
    auto bottomSliderWidth = bottomRowArea.getWidth() / 2;
    auto slot4 = bottomRowArea.removeFromLeft(bottomSliderWidth);
    auto slot5 = bottomRowArea;

    // Sin Amplitude
    sinAmplitudeSliderLabel.setBounds(slot4.removeFromTop(20));
    sinAmplitudeSlider.setBounds(slot4.removeFromTop(SLIDER_SIZE).reduced(5, 0));

    // Sin Frequency
    sinFrequencySliderLabel.setBounds(slot5.removeFromTop(20));
    sinFrequencySlider.setBounds(slot5.removeFromTop(SLIDER_SIZE).reduced(5, 0));
}

void SpatialSaturatorAudioProcessorEditor::layoutOutputSection(juce::Rectangle<int> bounds)
{
    bounds.removeFromTop(35); // Remove title space
    bounds.reduce(SECTION_PADDING, SECTION_PADDING);

    // Center the single control
    auto centerArea = bounds.reduced(bounds.getWidth() / 4, 0);

    // Make Up Gain - larger knob for output
    makeUpGainSliderLabel.setBounds(centerArea.removeFromTop(20));
    auto knobArea = centerArea.removeFromTop(SLIDER_SIZE + 20);
    makeUpGainSlider.setBounds(knobArea);
}