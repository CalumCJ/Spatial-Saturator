/*
  ==============================================================================

    This file contains the basic framework code for the filter class

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "SpatialSaturatorFilter.h"

//==============================================================================
void Filter::setCutOffFrequency(float cutOffFrequency)
{
    m_cutOffFrequency = cutOffFrequency;
}

void Filter::setSampleRate(float sample_rate)
{
    m_sample_rate = sample_rate;
}

//==============================================================================

void MidShelfFilter::processFilter(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages, int numberSamples, std::atomic<float>* cutOffFrequency, std::atomic<float>* gain)
{
    // mid shelf filter parameters
    auto w0b = 2 * juce::MathConstants<float>::pi * ((double)*cutOffFrequency / Filter::m_sample_rate);
    auto alpha_b = sin(w0b) / (2 * Filter::Q);
    double midGain = pow(10.0, (double)*gain * 0.025);

    double mid_in{}, mid_out{};

    auto b0bs = midGain * ((midGain + 1) - (midGain - 1) * cos(w0b) + 2 * sqrt(midGain) * alpha_b);
    auto b1bs = 2 * midGain * ((midGain - 1) - (midGain + 1) * cos(w0b));
    auto b2bs = midGain * ((midGain + 1) - (midGain - 1) * cos(w0b) - 2 * sqrt(midGain) * alpha_b);
    auto a0bs = (midGain + 1) + (midGain - 1) * cos(w0b) + 2 * sqrt(midGain) * alpha_b;
    auto a1bs = -2 * ((midGain - 1) + (midGain + 1) * cos(w0b));
    auto a2bs = (midGain + 1) + (midGain - 1) * cos(w0b) - 2 * sqrt(midGain) * alpha_b;

    b0bs = b0bs / a0bs; b1bs = b1bs / a0bs; b2bs = b2bs / a0bs; a1bs = a1bs / a0bs; a2bs = a2bs / a0bs; a0bs = a0bs / a0bs;

    for (int n = 0; n < numberSamples; ++n)
    {
        // Read mid channel
        mid_in = (double)buffer.getSample(0, n);

        // Apply low shelf to mid
        mid_out = (mid_in * b0bs) + m_m1;
        m_m1 = m_m2 + (mid_in * b1bs) - (mid_out * a1bs);
        m_m2 = (mid_in * b2bs) - (mid_out * a2bs);

        // Set output (mid to left channel)
        buffer.setSample(0, n, mid_out);
    }
}

void SideShelfFilter::processFilter(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&, int numberSamples, std::atomic<float>* cutOffFrequency, std::atomic<float>* gain)
{
    // Space shelf parameters
    auto w0ss = 2 * juce::MathConstants<float>::pi * ((double)*cutOffFrequency / Filter::m_sample_rate);
    auto alpha_ss = sin(w0ss) / (2 * Q);
    auto spaceGain = pow(10.0, (double)*gain * 0.025);

    double side_in{}, side_out{};

    auto b0ss = spaceGain * ((spaceGain + 1) - (spaceGain - 1) * cos(w0ss) + 2 * sqrt(spaceGain) * alpha_ss);
    auto b1ss = 2 * spaceGain * ((spaceGain - 1) - (spaceGain + 1) * cos(w0ss));
    auto b2ss = spaceGain * ((spaceGain + 1) - (spaceGain - 1) * cos(w0ss) - 2 * sqrt(spaceGain) * alpha_ss);
    auto a0ss = (spaceGain + 1) + (spaceGain - 1) * cos(w0ss) + 2 * sqrt(spaceGain) * alpha_ss;
    auto a1ss = -2 * ((spaceGain - 1) + (spaceGain + 1) * cos(w0ss));
    auto a2ss = (spaceGain + 1) + (spaceGain - 1) * cos(w0ss) - 2 * sqrt(spaceGain) * alpha_ss;

    b0ss = b0ss / a0ss; b1ss = b1ss / a0ss; b2ss = b2ss / a0ss; a1ss = a1ss / a0ss; a2ss = a2ss / a0ss; a0ss = a0ss / a0ss;

    for (int n = 0; n < numberSamples; ++n)
    {
        // Read side channel 
        side_in = (double)buffer.getSample(1, n);

        // Apply low-shelf to sides
        side_out = (side_in * b0ss) + m_s1;
        m_s1 = m_s2 + (side_in * b1ss) - (side_out * a1ss);
        m_s2 = (side_in * b2ss) - (side_out * a2ss);

        // Set output (side to right channel)
        buffer.setSample(1, n, side_out);
    }
}

void SideHpFilter::processFilter(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages, int numberSamples, std::atomic<float>* cutOffFrequency)
{
    // Space high_pass parameters
    auto w0shp = 2 * juce::MathConstants<float>::pi * ((double)*cutOffFrequency / Filter::m_sample_rate);
    auto alpha_shp = sin(w0shp) / (2 * Q);

    double side_in{}, side_out{};

    auto b0shp = (1 + cos(w0shp)) / 2;
    auto b1shp = -(1 + cos(w0shp));
    auto b2shp = (1 + cos(w0shp)) / 2;
    auto a0shp = 1 + alpha_shp;
    auto a1shp = -2 * cos(w0shp);
    auto a2shp = 1 - alpha_shp;

    b0shp = b0shp / a0shp; b1shp = b1shp / a0shp; b2shp = b2shp / a0shp; a1shp = a1shp / a0shp; a2shp = a2shp / a0shp; a0shp = a0shp / a0shp;


    for (int n = 0; n < numberSamples; ++n)
    {
        // Read side channel 
        side_in = (double)buffer.getSample(1, n);

        // Apply high pass to sides
        side_out = (side_in * b0shp) + m_shp1;
        m_shp1 = m_shp2 + (side_in * b1shp) - (side_out * a1shp);
        m_shp2 = (side_in * b2shp) - (side_out * a2shp);

        // Set output (side to right channel)
        buffer.setSample(1, n, side_out);
    }
}
