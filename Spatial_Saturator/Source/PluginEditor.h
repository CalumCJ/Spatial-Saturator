/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
// Custom LookAndFeel for professional appearance
class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel()
    {
        // Professional color scheme
        setColour(juce::Slider::thumbColourId, juce::Colour(0xff4a9eff));
        setColour(juce::Slider::trackColourId, juce::Colour(0xff3a3a3a));
        setColour(juce::Slider::backgroundColourId, juce::Colour(0xff1a1a1a));
        setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
        setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff2a2a2a));
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff1a1a1a));
        setColour(juce::Label::textColourId, juce::Colour(0xffc0c0c0));
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
        float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
        juce::Slider& slider) override
    {
        auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(10);
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto lineW = juce::jmin(8.0f, radius * 0.5f);
        auto arcRadius = radius - lineW * 0.5f;
        auto centre = bounds.getCentre();

        // Background arc
        juce::Path backgroundArc;
        backgroundArc.addCentredArc(centre.x, centre.y, arcRadius, arcRadius,
            0.0f, rotaryStartAngle, rotaryEndAngle, true);

        g.setColour(findColour(juce::Slider::backgroundColourId));
        g.strokePath(backgroundArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved,
            juce::PathStrokeType::rounded));

        // Value arc
        if (slider.isEnabled())
        {
            juce::Path valueArc;
            valueArc.addCentredArc(centre.x, centre.y, arcRadius, arcRadius,
                0.0f, rotaryStartAngle, toAngle, true);

            g.setColour(findColour(juce::Slider::thumbColourId));
            g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved,
                juce::PathStrokeType::rounded));
        }

        // Thumb
        auto thumbWidth = lineW * 2.0f;
        juce::Point<float> thumbPoint(centre.x + arcRadius * std::cos(toAngle - juce::MathConstants<float>::halfPi),
            centre.y + arcRadius * std::sin(toAngle - juce::MathConstants<float>::halfPi));

        g.setColour(findColour(juce::Slider::thumbColourId));
        g.fillEllipse(juce::Rectangle<float>(thumbWidth, thumbWidth).withCentre(thumbPoint));
    }
};

//==============================================================================
// Section Component for grouping controls
class SectionComponent : public juce::Component
{
public:
    SectionComponent(const juce::String& name) : sectionName(name) {}

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();

        // Background
        g.setColour(juce::Colour(0xff2a2a2a));
        g.fillRoundedRectangle(bounds, 8.0f);

        // Border
        g.setColour(juce::Colour(0xff3a3a3a));
        g.drawRoundedRectangle(bounds, 8.0f, 1.0f);

        // Title
        g.setColour(juce::Colours::white);
        g.setFont(juce::Font(16.0f, juce::Font::bold));
        g.drawText(sectionName, bounds.removeFromTop(35), juce::Justification::centred);
    }

private:
    juce::String sectionName;
};

//==============================================================================
// Simple Level Meter for visual feedback
class SimpleLevelMeter : public juce::Component, public juce::Timer
{
public:
    SimpleLevelMeter()
    {
        startTimerHz(30);
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat().reduced(2);

        // Background
        g.setColour(juce::Colour(0xff1a1a1a));
        g.fillRoundedRectangle(bounds, 2.0f);

        // Level bar
        auto levelHeight = bounds.getHeight() * currentLevel;
        auto levelBounds = bounds.removeFromBottom(levelHeight);

        // Gradient for level
        juce::ColourGradient gradient(juce::Colour(0xff4a9eff), levelBounds.getX(), levelBounds.getBottom(),
            juce::Colour(0xff6ab7ff), levelBounds.getX(), levelBounds.getY(), false);

        if (currentLevel > 0.8f)
            gradient.setColour(1, juce::Colour(0xffff4444));

        g.setGradientFill(gradient);
        g.fillRoundedRectangle(levelBounds, 2.0f);
    }

    void setLevel(float newLevel)
    {
        currentLevel = juce::jlimit(0.0f, 1.0f, newLevel);
    }

    void timerCallback() override
    {
        if (currentLevel > 0.01f)
        {
            currentLevel *= 0.95f;
            repaint();
        }
    }

private:
    float currentLevel = 0.0f;
};

//==============================================================================
/**
*/
class SpatialSaturatorAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    SpatialSaturatorAudioProcessorEditor(SpatialSaturatorAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~SpatialSaturatorAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // Constants for consistent spacing
    static constexpr int MARGIN = 20;
    static constexpr int SLIDER_SIZE = 80;
    static constexpr int SECTION_PADDING = 15;
    static constexpr float HEADER_FONT_SIZE = 24.0f;
    static constexpr float LABEL_FONT_SIZE = 12.0f;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SpatialSaturatorAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& treeState;

    // Custom Look and Feel
    CustomLookAndFeel customLookAndFeel;

    // Section Components
    SectionComponent midSection{ "MID PROCESSING" };
    SectionComponent sideSection{ "SIDE PROCESSING" };
    SectionComponent saturationSection{ "SATURATION" };
    SectionComponent outputSection{ "OUTPUT" };

    // Level Meters
    SimpleLevelMeter inputMeterL, inputMeterR;
    SimpleLevelMeter outputMeterL, outputMeterR;

    // Mid Section Controls
    juce::Slider midGainSlider;
    juce::Label midGainSliderLabel;
    juce::Slider midFreqSlider;
    juce::Label midFreqSliderLabel;

    // Side Section Controls
    juce::Slider sideGainSlider;
    juce::Label sideGainSliderLabel;
    juce::Slider sideFreqLowerSlider;
    juce::Label sideFreqLowerSliderLabel;
    juce::Slider sideFreqUpperSlider;
    juce::Label sideFreqUpperSliderLabel;

    // Saturation Controls
    juce::Slider tanhAmplitudeSlider;
    juce::Label tanhAmplitudeSliderLabel;
    juce::Slider tanhSlopeSlider;
    juce::Label tanhSlopeSliderLabel;
    juce::Slider saturatorMixSlider;
    juce::Label saturatorMixSliderLabel;
    juce::Slider sinAmplitudeSlider;
    juce::Label sinAmplitudeSliderLabel;
    juce::Slider sinFrequencySlider;
    juce::Label sinFrequencySliderLabel;

    // Output Control
    juce::Slider makeUpGainSlider;
    juce::Label makeUpGainSliderLabel;

    // Attachment Types
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

    // Attachments
    std::unique_ptr<SliderAttachment> midGainSliderAttachment;
    std::unique_ptr<SliderAttachment> midFreqSliderAttachment;
    std::unique_ptr<SliderAttachment> sideGainSliderAttachment;
    std::unique_ptr<SliderAttachment> sideFreqLowerSliderAttachment;
    std::unique_ptr<SliderAttachment> sideFreqUpperSliderAttachment;
    std::unique_ptr<SliderAttachment> tanhAmplitudeSliderAttachment;
    std::unique_ptr<SliderAttachment> tanhSlopeSliderAttachment;
    std::unique_ptr<SliderAttachment> saturatorMixSliderAttachment;
    std::unique_ptr<SliderAttachment> sinAmplitudeSliderAttachment;
    std::unique_ptr<SliderAttachment> sinFrequencySliderAttachment;
    std::unique_ptr<SliderAttachment> makeUpGainSliderAttachment;

    // Helper functions
    void configureSlider(juce::Slider& slider, juce::Label& label, const juce::String& labelText,
        const juce::String& suffix, bool isRotary = true);
    void layoutMidSection(juce::Rectangle<int> bounds);
    void layoutSideSection(juce::Rectangle<int> bounds);
    void layoutSaturationSection(juce::Rectangle<int> bounds);
    void layoutOutputSection(juce::Rectangle<int> bounds);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpatialSaturatorAudioProcessorEditor)
};